#pragma once

#include "AI/BTNode.h"
#include "AI/AStarPathfinder.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"
#include "Actor/Character/Component/BubbleComponent.h"

using namespace engine;

// 플레이어로 가는 경로를 막는 박스를 찾아 이동한 뒤 물풍선을 설치한다.
//
// 동작 순서 (매 틱):
//   1. AStar(박스 투과)로 이상 경로를 탐색해 목표 배치 타일 계산
//   2. 배치 타일에 도달했으면 → 물풍선 설치 시도 (내부 쿨다운 적용)
//   3. 미도달 → AStar 경로로 한 칸 이동 후 Running 반환
class PlaceBubbleNearBoxAction : public engine::BTNode
{
public:
	PlaceBubbleNearBoxAction(Enemy* enemy, MovementComponent* movement, BubbleComponent* bubble)
		: enemy(enemy), movement(movement), bubble(bubble)
	{
	}

	engine::BTStatus Execute(float deltaTime) override
	{
		if (!enemy || !movement || !bubble) return engine::BTStatus::Failure;
		if (movement->IsMoving()) return engine::BTStatus::Running;

		cooldownElapsed += deltaTime;

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
			return engine::BTStatus::Failure;

		// 배치 타일에 도달했으면 물풍선 설치
		if (start == placementTile)
		{
			if (cooldownElapsed >= BUBBLE_COOLDOWN)
			{
				if (bubble->RequestGenerateBubble())
				{
					cooldownElapsed = 0.0f;
					return engine::BTStatus::Success;
				}
			}
			return engine::BTStatus::Running;
		}

		// 배치 타일까지 AStar로 한 칸 이동
		Vector2 nextPos = AStarPathfinder::GetNextStep(start, placementTile, canMoveNormal, ts);
		if (nextPos == Vector2::Zero)
			return engine::BTStatus::Failure;

		Vector2 dir(
			(nextPos.x - start.x) / ts,
			(nextPos.y - start.y) / ts
		);
		movement->RequestMove(dir);
		return engine::BTStatus::Running;
	}

private:
	static constexpr float BUBBLE_COOLDOWN = 3.0f;

	Enemy*            enemy;
	MovementComponent* movement;
	BubbleComponent*   bubble;
	float cooldownElapsed = 0.0f;
};
