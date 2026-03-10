#pragma once

#include "AI/BTNode.h"
#include "Actor/Character/Enemy/AI/AStarPathfinder.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"
#include "Actor/Character/Component/WaterBalloonComponent.h"
#include "Actor/Character/Enemy/AI/BT/Actions/EscapeHelper.h"

using namespace engine;

namespace enemy_ai
{
	class PlaceWaterBalloonNearBoxAction : public BTNode
	{
	public:
		PlaceWaterBalloonNearBoxAction(Enemy* enemy, MovementComponent* movement, WaterBalloonComponent* waterBalloon)
			: enemy(enemy), movement(movement), waterBalloon(waterBalloon)
		{
		}

		BTStatus Execute(float) override
		{
			if (!enemy || !movement || !waterBalloon) return BTStatus::Failure;
			if (movement->IsMoving()) return BTStatus::Running;

			const Vector2 start = enemy->GetPosition();
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

			Vector2 placementTile = AStarPathfinder::FindBoxPlacementTile(
				start, target,
				canMoveNormal, canMoveThroughBoxes, hasBox,
				ts
			);

			if (placementTile == Vector2::Zero)
				return BTStatus::Failure;

			if (start == placementTile)
			{
				if (waterBalloon->RequestGenerateWaterBalloon())
				{
					EvaluateAndPrepareEscape(start, ts);
					return BTStatus::Success;
				}
				return BTStatus::Running;
			}

			Vector2 nextPos = AStarPathfinder::GetNextStep(start, placementTile, canMoveNormal, ts);
			if (nextPos == Vector2::Zero)
				return BTStatus::Failure;

			if (enemy->QueryIsExplosionDangerAt(nextPos))
				return BTStatus::Failure;

			Vector2 dir(
				nextPos.x / ts - start.x / ts,
				nextPos.y / ts - start.y / ts
			);
			
			enemy->ClearEscapePath();
			movement->RequestMove(dir);
			return BTStatus::Running;
		}

	private:
		void EvaluateAndPrepareEscape(const Vector2& placementPos, int ts)
		{
			if (!EscapeHelper::ShouldKeepEscapePath(enemy, placementPos, ts))
				enemy->ClearEscapePath();
		}

		Enemy* enemy;
		MovementComponent* movement;
		WaterBalloonComponent* waterBalloon;
	};
}
