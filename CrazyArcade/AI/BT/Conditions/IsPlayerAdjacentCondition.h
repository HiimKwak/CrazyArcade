#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"

using namespace engine;

// 플레이어가 같은 행/열로 attackRange 타일 이내에 있으면 Success
// (물풍선 폭발 경로로 타격 가능한 위치)
class IsPlayerAdjacentCondition : public engine::BTNode
{
public:
	explicit IsPlayerAdjacentCondition(Enemy* enemy, int attackRangeTiles = 3)
		: enemy(enemy), attackRangeTiles(attackRangeTiles)
	{
	}

	engine::BTStatus Execute(float) override
	{
		if (!enemy) return engine::BTStatus::Failure;

		const Vector2 playerPos = enemy->QueryPlayerPosition();
		const Vector2 enemyPos  = enemy->GetPosition();
		const int ts = Engine::Get().GetTileSize();
		const Vector2 delta = playerPos - enemyPos;

		if (delta.y == 0 && abs(delta.x) <= attackRangeTiles * ts)
			return engine::BTStatus::Success;

		if (delta.x == 0 && abs(delta.y) <= attackRangeTiles * ts)
			return engine::BTStatus::Success;

		return engine::BTStatus::Failure;
	}

private:
	Enemy* enemy;
	int attackRangeTiles;
};
