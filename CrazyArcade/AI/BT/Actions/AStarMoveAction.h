#pragma once

#include "AI/BTNode.h"
#include "AI/AStarPathfinder.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"

using namespace engine;

// A* 최단 경로로 플레이어를 향해 한 칸 이동하고, 경로를 Enemy에 저장해 시각화한다.
// 경로가 존재하지 않으면(박스에 막힌 경우) Failure 반환.
class AStarMoveAction : public engine::BTNode
{
public:
	AStarMoveAction(Enemy* enemy, MovementComponent* movement)
		: enemy(enemy), movement(movement)
	{
	}

	engine::BTStatus Execute(float) override
	{
		if (!enemy || !movement) return engine::BTStatus::Failure;

		const Vector2 start  = enemy->GetPosition();
		const Vector2 target = enemy->QueryPlayerPosition();
		const int ts = Engine::Get().GetTileSize();

		auto canMove = [this](const Vector2& from, const Vector2& to)
		{
			return enemy->QueryCanMove(from, to);
		};

		std::vector<Vector2> path = AStarPathfinder::GetFullPath(start, target, canMove, ts);

		// 시각화: 박스를 무시한 A* 최단 경로를 표시
		auto canMoveIgnoreBoxes = [this](const Vector2& from, const Vector2& to)
		{
			return enemy->QueryCanMove(from, to) || enemy->QueryHasBoxAt(to);
		};
		enemy->SetDebugPath(AStarPathfinder::GetFullPath(start, target, canMoveIgnoreBoxes, ts));

		if (path.size() < 2)
			return engine::BTStatus::Failure;

		const Vector2& nextPos = path[1];
		if (enemy->QueryIsExplosionDangerAt(nextPos))
			return engine::BTStatus::Failure;

		Vector2 dir(
			nextPos.x / ts - start.x / ts,
			nextPos.y / ts - start.y / ts
		);
		movement->RequestMove(dir);
		return engine::BTStatus::Success;
	}

private:
	Enemy* enemy;
	MovementComponent* movement;
};
