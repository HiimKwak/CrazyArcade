#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"

using namespace engine;

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

		static const Vector2 dirs[4] = {
			Vector2::Right, Vector2::Left, Vector2::Up, Vector2::Down
		};

		for (const Vector2& dir : dirs)
		{
			Vector2 next = pos + dir * ts;
			if (enemy->QueryCanMove(pos, next) && !enemy->QueryHasBubbleAt(next))
			{
				movement->RequestMove(dir);
				return engine::BTStatus::Success;
			}
		}

		return engine::BTStatus::Failure;
	}

private:
	Enemy* enemy;
	MovementComponent* movement;
};
