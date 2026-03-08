#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"

using namespace engine;

class IsPlayerAdjacentCondition : public BTNode
{
public:
	explicit IsPlayerAdjacentCondition(Enemy* enemy, int attackRangeTiles = 3)
		: enemy(enemy), attackRangeTiles(attackRangeTiles)
	{
	}

	BTStatus Execute(float) override
	{
		if (!enemy) return BTStatus::Failure;

		const Vector2 playerPos = enemy->QueryPlayerPosition();
		const Vector2 enemyPos  = enemy->GetPosition();
		const int ts = Engine::Get().GetTileSize();
		const Vector2 delta = playerPos - enemyPos;

		if (delta.y == 0 && abs(delta.x) <= attackRangeTiles * ts)
			return BTStatus::Success;

		if (delta.x == 0 && abs(delta.y) <= attackRangeTiles * ts)
			return BTStatus::Success;

		return BTStatus::Failure;
	}

private:
	Enemy* enemy;
	int attackRangeTiles;
};
