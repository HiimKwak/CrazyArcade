#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Actor/Character/Enemy/Enemy.h"

using namespace engine;

// 현재 위치가 임의 물풍선의 예상 폭발 범위 안에 있으면 Success.
// 폭발 범위는 물풍선 배치 시점에 벽/박스 차단을 반영해 사전 계산된다.
class IsDangerNearbyCondition : public engine::BTNode
{
public:
	explicit IsDangerNearbyCondition(Enemy* enemy)
		: enemy(enemy)
	{
	}

	engine::BTStatus Execute(float) override
	{
		if (!enemy) return engine::BTStatus::Failure;

		if (enemy->QueryIsExplosionDangerAt(enemy->GetPosition()))
			return engine::BTStatus::Success;

		return engine::BTStatus::Failure;
	}

private:
	Enemy* enemy;
};
