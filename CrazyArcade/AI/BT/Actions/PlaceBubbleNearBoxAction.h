#pragma once

#include "AI/BTNode.h"
#include "AI/AStarPathfinder.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"
#include "Actor/Character/Component/BubbleComponent.h"

using namespace engine;

class PlaceBubbleNearBoxAction : public BTNode
{
public:
	PlaceBubbleNearBoxAction(Enemy* enemy, MovementComponent* movement, BubbleComponent* bubble)
		: enemy(enemy), movement(movement), bubble(bubble)
	{
	}

	BTStatus Execute(float) override
	{
		if (!enemy || !movement || !bubble) return BTStatus::Failure;
		if (movement->IsMoving()) return BTStatus::Running;

		const Vector2 start  = enemy->GetPosition();
		const Vector2 target = enemy->QueryPlayerPosition();
		const int ts = Engine::Get().GetTileSize();

		auto canMoveNormal = [this](const Vector2& f, const Vector2& t)
		{
			return enemy->QueryCanMove(f, t);
		};
		auto canMoveThroughBoxes = [this](const Vector2& f, const Vector2& t)
		{
			return enemy->QueryCanMove(f, t) || enemy->QueryHasBoxAt(t);
		};
		auto hasBox = [this](const Vector2& p)
		{
			return enemy->QueryHasBoxAt(p);
		};

		// 배치 타일 계산 (이미 경로가 열려 있으면 Zero 반환)
		Vector2 placementTile = AStarPathfinder::FindBoxPlacementTile(
			start, target,
			canMoveNormal, canMoveThroughBoxes, hasBox,
			ts
		);

		if (placementTile == Vector2::Zero)
			return BTStatus::Failure;

		// 배치 타일에 도달했으면 물풍선 설치
		if (start == placementTile)
		{
			if (bubble->RequestGenerateBubble())
				return BTStatus::Success;
			return BTStatus::Running;
		}

		// 배치 타일까지 AStar로 한 칸 이동
		Vector2 nextPos = AStarPathfinder::GetNextStep(start, placementTile, canMoveNormal, ts);
		if (nextPos == Vector2::Zero)
			return BTStatus::Failure;

		if (enemy->QueryIsExplosionDangerAt(nextPos))
			return BTStatus::Failure;

		Vector2 dir(
			nextPos.x / ts - start.x / ts,
			nextPos.y / ts - start.y / ts
		);
		movement->RequestMove(dir);
		return BTStatus::Running;
	}

private:
	Enemy*            enemy;
	MovementComponent* movement;
	BubbleComponent*   bubble;
};
