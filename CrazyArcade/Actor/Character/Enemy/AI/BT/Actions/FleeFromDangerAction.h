#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"
#include "Actor/MoveSpeed.h"
#include <queue>
#include <unordered_set>
#include <utility>
#include <limits>

using namespace engine;

namespace enemy_ai
{
	class FleeFromDangerAction : public engine::BTNode
	{
	public:
		FleeFromDangerAction(Enemy* enemy, MovementComponent* movement)
			: enemy(enemy), movement(movement)
		{
		}

		engine::BTStatus Execute(float) override
		{
			if (!enemy || !movement) return engine::BTStatus::Failure;
			if (movement->IsMoving())  return engine::BTStatus::Running;

			const Vector2 pos = enemy->GetPosition();
			const int ts = Engine::Get().GetTileSize();

			Vector2 dir = FindEscapeDir(pos, ts);
			if (dir == Vector2::Zero)
				return engine::BTStatus::Failure;

			movement->RequestMove(dir);
			return engine::BTStatus::Success;
		}

	private:
		static constexpr int MAX_DEPTH = 8;
		static constexpr float DANGER_TIME_MARGIN = 0.05f;

		using TileKey = std::pair<int, int>;

		struct TileKeyHash
		{
			size_t operator()(const TileKey& k) const noexcept
			{
				size_t h1 = std::hash<int>{}(k.first);
				size_t h2 = std::hash<int>{}(k.second);
				return h1 ^ (h2 << 1);
			}
		};

		struct SearchNode
		{
			Vector2 pos;
			Vector2 firstDir;
			int depth;
		};

		TileKey Key(const Vector2& v, int ts) const
		{
			return { v.x / ts, v.y / ts };
		}

		int CountAdjacentBoxes(const Vector2& pos, int ts) const
		{
			static const Vector2 dirs[4] = {
				Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
			};

			int count = 0;
			for (const Vector2& dir : dirs)
			{
				if (enemy->QueryHasBoxAt(pos + dir * ts))
					++count;
			}
			return count;
		}

		// returns the count of walkable and safe tiles
		int CountSafeExits(const Vector2& pos, int ts) const
		{
			static const Vector2 dirs[4] = {
				Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
			};

			int count = 0;
			for (const Vector2& dir : dirs)
			{
				Vector2 next = pos + dir * ts;
				if (!enemy->QueryCanMove(pos, next))
					continue;
				if (!enemy->QueryIsExplosionDangerAt(next))
					++count;
			}
			return count;
		}

		// evaluate if next tile is worthwhile to enter or not. returns false if no adjacent boxes and immediate exits
		bool IsUsefulDangerTraversal(const Vector2& next, int ts) const
		{
			return CountAdjacentBoxes(next, ts) > 0 && CountSafeExits(next, ts) > 0;
		}

		// returns actual amount of worth enterting danger site
		int EvaluateSafeTileValue(const Vector2& pos, int ts) const
		{
			return CountSafeExits(pos, ts) * 10 + CountAdjacentBoxes(pos, ts);
		}

		bool CanTraverseDangerTile(const Vector2& next, int nextDepth, int ts, bool forceEscapeFromPlacedBubble) const
		{
			if (!forceEscapeFromPlacedBubble && !IsUsefulDangerTraversal(next, ts))
				return false;

			float arrivalTime = nextDepth * MoveSpeed::NORMAL;
			float explosionTime = enemy->QueryExplosionTimeAt(next);
			return arrivalTime + DANGER_TIME_MARGIN < explosionTime;
		}

		// Search the nearest safe tile with BFS
		Vector2 SearchEscapeDir(
			const Vector2& start, int ts, bool allowTimedDangerTraversal, bool forceEscapeFromPlacedBubble
		) const
		{
			static const Vector2 dirs[4] = {
				Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
			};

			std::queue<SearchNode> q;
			std::unordered_set<TileKey, TileKeyHash> visited;

			q.push({ start, Vector2::Zero, 0 });
			visited.insert(Key(start, ts));

			Vector2 bestDir = Vector2::Zero;
			int bestDepth = (std::numeric_limits<int>::max)();
			int bestValue = (std::numeric_limits<int>::min)();

			while (!q.empty())
			{
				SearchNode node = q.front();
				q.pop();

				if (node.depth >= MAX_DEPTH) continue;
				if (node.depth > bestDepth) continue;

				for (const Vector2& dir : dirs)
				{
					Vector2 next = node.pos + dir * ts;
					TileKey nk = Key(next, ts);

					if (visited.find(nk) != visited.end())
						continue;
					if (!enemy->QueryCanMove(node.pos, next))
						continue;

					visited.insert(nk);

					Vector2 fd = (node.firstDir == Vector2::Zero)
						? dir
						: node.firstDir;
					int nextDepth = node.depth + 1;

					if (!enemy->QueryIsExplosionDangerAt(next))
					{
						int candValue = EvaluateSafeTileValue(next, ts);
						if (nextDepth < bestDepth || (nextDepth == bestDepth && candValue > bestValue))
						{
							bestDepth = nextDepth;
							bestValue = candValue;
							bestDir = fd;
						}
						continue;
					}

					// danger traverse guard(1. check whether allowed 2. judge whether it's worthwhile)
					if (!allowTimedDangerTraversal)
						continue;
					if (!CanTraverseDangerTile(next, nextDepth, ts, forceEscapeFromPlacedBubble))
						continue;

					q.push({ next, fd, nextDepth });
				}
			}

			return bestDir;
		}

		Vector2 FindEscapeDir(const Vector2& start, int ts) const
		{
			const bool forceEscapeFromPlacedBubble = enemy->QueryHasBubbleAt(start);

			Vector2 safeOnlyDir = SearchEscapeDir(start, ts, false, forceEscapeFromPlacedBubble);
			if (safeOnlyDir != Vector2::Zero)
				return safeOnlyDir;

			return SearchEscapeDir(start, ts, true, forceEscapeFromPlacedBubble);
		}

		Enemy* enemy;
		MovementComponent* movement;
	};
}
