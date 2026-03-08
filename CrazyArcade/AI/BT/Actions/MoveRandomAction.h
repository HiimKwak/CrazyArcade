#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Util/Util.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"

using namespace engine;

class MoveRandomAction : public engine::BTNode
{
public:
	MoveRandomAction(Enemy* enemy, MovementComponent* movement)
		: enemy(enemy), movement(movement)
	{
	}

	engine::BTStatus Execute(float) override
	{
		if (!enemy || !movement)
			return engine::BTStatus::Failure;

		const Vector2 enemyPos = enemy->GetPosition();
		const int ts = Engine::Get().GetTileSize();

		static const Vector2 dirs[4] = { Vector2::Right, Vector2::Left, Vector2::Up, Vector2::Down };

		// 랜덤 방향 우선 시도
		for (int i = 0; i < 4; ++i)
		{
			const Vector2& dir = dirs[Util::Random(0, 4)];
			if (TryMove(dir, enemyPos, ts))
				return engine::BTStatus::Success;
		}

		// 결정론적 폴백
		for (const Vector2& dir : dirs)
		{
			if (TryMove(dir, enemyPos, ts))
				return engine::BTStatus::Success;
		}

		return engine::BTStatus::Failure;
	}

private:
	bool TryMove(const Vector2& dir, const Vector2& from, int tileSize)
	{
		const Vector2 next = from + dir * tileSize;
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
