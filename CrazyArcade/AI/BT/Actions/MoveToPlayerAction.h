#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"

using namespace engine;

class MoveToPlayerAction : public engine::BTNode
{
public:
	MoveToPlayerAction(Enemy* enemy, MovementComponent* movement)
		: enemy(enemy), movement(movement)
	{
	}

	engine::BTStatus Execute(float) override
	{
		if (!enemy || !movement)
			return engine::BTStatus::Failure;

		const Vector2 enemyPos = enemy->GetPosition();
		const Vector2 playerPos = enemy->QueryPlayerPosition();
		const Vector2 delta = playerPos - enemyPos;

		Vector2 primary, secondary;
		if (abs(static_cast<int>(delta.x)) >= abs(static_cast<int>(delta.y)))
		{
			primary   = delta.x >= 0 ? Vector2::Right : Vector2::Left;
			secondary = delta.y >= 0 ? Vector2::Down  : Vector2::Up;
		}
		else
		{
			primary   = delta.y >= 0 ? Vector2::Down  : Vector2::Up;
			secondary = delta.x >= 0 ? Vector2::Right : Vector2::Left;
		}

		if (TryMove(primary, enemyPos))   return engine::BTStatus::Success;
		if (TryMove(secondary, enemyPos)) return engine::BTStatus::Success;

		return engine::BTStatus::Failure;
	}

private:
	bool TryMove(const Vector2& dir, const Vector2& from)
	{
		if (dir == Vector2::Zero) return false;
		const int ts = Engine::Get().GetTileSize();
		const Vector2 next = from + dir * ts;
		if (enemy->QueryCanMove(from, next))
		{
			movement->RequestMove(dir);
			return true;
		}
		return false;
	}

	Enemy* enemy;
	MovementComponent* movement;
};
