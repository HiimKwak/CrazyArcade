#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include <limits>

using namespace engine;

namespace enemy_ai
{
class IsPlayerInDistanceRangeCondition : public BTNode
{
public:
	IsPlayerInDistanceRangeCondition(Enemy* enemy, int minTiles, int maxTiles = (std::numeric_limits<int>::max)())
		: enemy(enemy), minTiles(minTiles), maxTiles(maxTiles)
	{
	}

	BTStatus Execute(float) override
	{
		if (!enemy) return BTStatus::Failure;

		const Vector2 playerPos = enemy->QueryPlayerPosition();
		const Vector2 enemyPos = enemy->GetPosition();
		const int ts = Engine::Get().GetTileSize();

		const int dx = abs(playerPos.x / ts - enemyPos.x / ts);
		const int dy = abs(playerPos.y / ts - enemyPos.y / ts);
		const int dist = dx + dy;

		if (dist < minTiles || dist > maxTiles)
			return BTStatus::Failure;
		return BTStatus::Success;
	}

private:
	Enemy* enemy;
	int minTiles;
	int maxTiles;
};
}
