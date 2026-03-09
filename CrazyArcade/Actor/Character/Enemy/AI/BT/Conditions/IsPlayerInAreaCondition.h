#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include <cstdlib>

using namespace engine;

namespace enemy_ai
{
class IsPlayerInAreaCondition : public BTNode
{
public:
	IsPlayerInAreaCondition(Enemy* enemy, int minTilesInclusive, int maxTilesInclusive = -1)
		: enemy(enemy), minTilesInclusive(minTilesInclusive), maxTilesInclusive(maxTilesInclusive)
	{
	}

	BTStatus Execute(float) override
	{
		if (!enemy) return BTStatus::Failure;

		const Vector2 enemyPos = enemy->GetPosition();
		const Vector2 playerPos = enemy->QueryPlayerPosition();
		const int ts = Engine::Get().GetTileSize();

		const Vector2 delta = playerPos - enemyPos;
		const int distTiles = (abs(static_cast<int>(delta.x)) + abs(static_cast<int>(delta.y))) / ts;

		if (distTiles < minTilesInclusive)
			return BTStatus::Failure;
		if (maxTilesInclusive >= 0 && distTiles > maxTilesInclusive)
			return BTStatus::Failure;

		return BTStatus::Success;
	}

private:
	Enemy* enemy;
	int minTilesInclusive;
	int maxTilesInclusive;
};
}
