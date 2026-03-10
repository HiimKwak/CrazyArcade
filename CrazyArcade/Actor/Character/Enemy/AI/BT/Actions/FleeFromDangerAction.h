#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <limits>
#include <algorithm>

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

			const auto& escapePath = enemy->GetEscapePath();
			if (!escapePath.empty())
			{
				if (escapePath.front() == pos)
				{
					auto updatedPath = escapePath;
					updatedPath.erase(updatedPath.begin());
					enemy->SetEscapePath(std::move(updatedPath));
				}

				const auto& currentPath = enemy->GetEscapePath();
				if (!currentPath.empty())
				{
					Vector2 nextPos = currentPath.front();
					Vector2 dir(
						(nextPos.x - pos.x) / ts,
						(nextPos.y - pos.y) / ts
					);

					if (enemy->QueryIsExplosionDangerAt(nextPos))
					{
						float explosionTime = enemy->QueryExplosionTimeAt(nextPos);
						float currentSpeed = movement->GetMoveSpeed();
						if (currentSpeed + DANGER_TIME_MARGIN >= explosionTime)
						{
							enemy->ClearEscapePath();
						}
						else
						{
							movement->RequestMove(dir);
							return engine::BTStatus::Running;
						}
					}
					else
					{
						movement->RequestMove(dir);
						return engine::BTStatus::Running;
					}
				}
			}

			std::vector<Vector2> newPath = FindEscapePath(pos, ts);
			if (newPath.empty())
				return engine::BTStatus::Failure;

			enemy->SetEscapePath(newPath);

			Vector2 nextPos = newPath.front();
			Vector2 dir(
				(nextPos.x - pos.x) / ts,
				(nextPos.y - pos.y) / ts
			);
			movement->RequestMove(dir);
			return engine::BTStatus::Success;
		}

	private:
		static constexpr int MAX_DEPTH = 15;
		static constexpr float DANGER_TIME_MARGIN = 0.1f;

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
			int depth;
			float arrivalTime;
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

		bool CanTraverseDangerTile(const Vector2& next, float arrivalTime, int ts, bool forceEscape) const
		{
			if (!forceEscape && !IsUsefulDangerTraversal(next, ts))
				return false;

			float explosionTime = enemy->QueryExplosionTimeAt(next);
			return arrivalTime + DANGER_TIME_MARGIN < explosionTime;
		}

		std::vector<Vector2> ReconstructPath(
			const std::unordered_map<TileKey, TileKey, TileKeyHash>& cameFrom,
			TileKey start, TileKey goal, int ts
		) const
		{
			std::vector<Vector2> path;
			TileKey current = goal;

			while (current != start)
			{
				path.push_back(Vector2(current.first * ts, current.second * ts));
				auto it = cameFrom.find(current);
				if (it == cameFrom.end())
					break;
				current = it->second;
			}

			std::reverse(path.begin(), path.end());
			return path;
		}

		std::vector<Vector2> SearchEscapePath(
			const Vector2& start, int ts, bool allowTimedDangerTraversal, bool forceEscapeFromPlacedWB
		) const
		{
			static const Vector2 dirs[4] = {
				Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
			};

			float currentSpeed = movement ? movement->GetMoveSpeed() : MovementComponent::SPEED_NORMAL;

			std::queue<SearchNode> q;
			std::unordered_set<TileKey, TileKeyHash> visited;
			std::unordered_map<TileKey, TileKey, TileKeyHash> cameFrom;

			TileKey startKey = Key(start, ts);
			q.push({ start, 0, 0.0f });
			visited.insert(startKey);

			Vector2 bestSafePos = Vector2::Zero;
			int bestDepth = (std::numeric_limits<int>::max)();
			int bestValue = (std::numeric_limits<int>::min)();

			while (!q.empty())
			{
				SearchNode node = q.front();
				q.pop();

				if (node.depth >= MAX_DEPTH) continue;
				if (node.depth > bestDepth) continue;

				TileKey nodeKey = Key(node.pos, ts);

				for (const Vector2& dir : dirs)
				{
					Vector2 next = node.pos + dir * ts;
					TileKey nextKey = Key(next, ts);

					if (visited.find(nextKey) != visited.end())
						continue;
					if (!enemy->QueryCanMove(node.pos, next))
						continue;

					visited.insert(nextKey);
					cameFrom[nextKey] = nodeKey;

					int nextDepth = node.depth + 1;
					float nextArrivalTime = node.arrivalTime + currentSpeed;

					if (!enemy->QueryIsExplosionDangerAt(next))
					{
						int candValue = EvaluateSafeTileValue(next, ts);
						if (nextDepth < bestDepth || (nextDepth == bestDepth && candValue > bestValue))
						{
							bestDepth = nextDepth;
							bestValue = candValue;
							bestSafePos = next;
						}
						continue;
					}

					if (!allowTimedDangerTraversal)
						continue;
					if (!CanTraverseDangerTile(next, nextArrivalTime, ts, forceEscapeFromPlacedWB))
						continue;

					q.push({ next, nextDepth, nextArrivalTime });
				}
			}

			if (bestSafePos == Vector2::Zero)
				return std::vector<Vector2>();

			return ReconstructPath(cameFrom, startKey, Key(bestSafePos, ts), ts);
		}

		std::vector<Vector2> FindEscapePath(const Vector2& start, int ts) const
		{
			const bool forceEscapeFromPlacedWB = enemy->QueryHasWaterBalloonAt(start);

			auto safeOnlyPath = SearchEscapePath(start, ts, false, forceEscapeFromPlacedWB);
			if (!safeOnlyPath.empty())
				return safeOnlyPath;

			return SearchEscapePath(start, ts, true, forceEscapeFromPlacedWB);
		}

		Enemy* enemy;
		MovementComponent* movement;
	};
}
