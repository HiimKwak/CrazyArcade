#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"

using namespace engine;

namespace enemy_ai
{
	class IsDangerNearbyCondition : public BTNode
	{
	public:
		explicit IsDangerNearbyCondition(Enemy* enemy)
			: enemy(enemy)
		{
		}

		BTStatus Execute(float) override
		{
			if (!enemy) return BTStatus::Failure;

			const Vector2 pos = enemy->GetPosition();
			const int ts = Engine::Get().GetTileSize();

			// Check current position
			if (enemy->QueryIsExplosionDangerAt(pos))
			{
				float explosionTime = enemy->QueryExplosionTimeAt(pos);
				auto movement = enemy->GetComponent<MovementComponent>();
				float moveSpeed = movement ? movement->GetMoveSpeed() : MovementComponent::SPEED_NORMAL;

				if (moveSpeed + DANGER_TIME_MARGIN >= explosionTime)
					return BTStatus::Success; // means not enough time to escape when arrived at next position
			}

			static const Vector2 dirs[4] = {
				Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
			};

			// Check nearby tiles (up to 3 tiles away)
			for (int dist = 1; dist <= DANGER_CHECK_RANGE; ++dist)
			{
				for (const Vector2& dir : dirs)
				{
					Vector2 checkPos = pos + dir * (ts * dist);
					if (enemy->QueryIsExplosionDangerAt(checkPos))
					{
						float explosionTime = enemy->QueryExplosionTimeAt(checkPos);
						auto movement = enemy->GetComponent<MovementComponent>();
						float moveSpeed = movement ? movement->GetMoveSpeed() : MovementComponent::SPEED_NORMAL;
						float arrivalTime = moveSpeed * dist;

						if (arrivalTime + DANGER_TIME_MARGIN >= explosionTime)
							return BTStatus::Success; // means dangerous within 3 tiles
					}
				}
			}

			return BTStatus::Failure;
		}

	private:
		static constexpr int DANGER_CHECK_RANGE = 3;
		static constexpr float DANGER_TIME_MARGIN = 0.5f;  // Safety buffer to allow escape after arrival

		Enemy* enemy;
	};
}
