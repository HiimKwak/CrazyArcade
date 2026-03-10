#pragma once

#include "AI/BTNode.h"
#include "Actor/Character/Enemy/AI/AStarPathfinder.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"

using namespace engine;

namespace enemy_ai
{
	class AStarMoveAction : public engine::BTNode
	{
	public:
		AStarMoveAction(Enemy* enemy, MovementComponent* movement)
			: enemy(enemy), movement(movement)
		{
		}

		engine::BTStatus Execute(float) override
		{
			if (!enemy || !movement) return engine::BTStatus::Failure;

			const Vector2 start = enemy->GetPosition();
			const Vector2 target = enemy->QueryPlayerPosition();
			const int ts = Engine::Get().GetTileSize();

			auto canMove = [this](const Vector2& from, const Vector2& to)
				{
					return enemy->QueryCanMove(from, to);
				};

			std::vector<Vector2> path = AStarPathfinder::GetFullPath(start, target, canMove, ts);

			auto canMoveIgnoreBoxes = [this](const Vector2& from, const Vector2& to)
				{
					return enemy->QueryCanMove(from, to) || enemy->QueryHasBoxAt(to);
				};
			enemy->SetDebugPath(AStarPathfinder::GetFullPath(start, target, canMoveIgnoreBoxes, ts)); // show ideal shortest path

			if (path.size() < 2) // 1 element means nowhere to move except where it is
				return engine::BTStatus::Failure;

			const Vector2& nextPos = path[1];
			if (enemy->QueryIsExplosionDangerAt(nextPos))
				return engine::BTStatus::Failure;

			Vector2 dir(
				nextPos.x / ts - start.x / ts,
				nextPos.y / ts - start.y / ts
			);
			
			enemy->ClearEscapePath();
			movement->RequestMove(dir);
			return engine::BTStatus::Success;
		}

	private:
		Enemy* enemy;
		MovementComponent* movement;
	};
}
