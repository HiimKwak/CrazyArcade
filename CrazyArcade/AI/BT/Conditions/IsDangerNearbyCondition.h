#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"

using namespace engine;

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

		if (enemy->QueryIsExplosionDangerAt(pos))
			return BTStatus::Success;

		static const Vector2 dirs[4] = {
			Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
		};
		for (const Vector2& dir : dirs)
		{
			if (enemy->QueryIsExplosionDangerAt(pos + dir * ts))
				return BTStatus::Success;
		}

		return BTStatus::Failure;
	}

private:
	Enemy* enemy;
};
