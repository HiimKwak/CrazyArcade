#pragma once

#include "Math/Vector2.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"
#include "Actor/Character/Component/StatsComponent.h"
#include <vector>

using namespace engine;

namespace enemy_ai
{
	// Shared escape evaluation logic for water balloon placement actions
	class EscapeHelper
	{
	public:
		static constexpr int UNREACHABLE_STEPS = 999;
		static constexpr int MAX_SEARCH_STEPS = 10;
		static constexpr float WATER_BALLOON_EXPLOSION_TIME = 3.0f;
		static constexpr float IMMEDIATE_ESCAPE_TIME_THRESHOLD = 0.5f;
		static constexpr int IMMEDIATE_ESCAPE_STEP_THRESHOLD = 2;

		// Find minimum steps to reach safety from own balloon's explosion range
		static int FindMinStepsToSafety(
			const Enemy* enemy,
			const Vector2& placementPos,
			int ts,
			int range)
		{
			static const Vector2 dirs[4] = {
				Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
			};

			// Build own danger zone
			std::vector<Vector2> ownDangerZone;
			ownDangerZone.push_back(placementPos);

			for (const Vector2& dir : dirs)
			{
				for (int i = 1; i <= range; ++i)
				{
					Vector2 dangerTile = placementPos + dir * (i * ts);

					if (!enemy->QueryCanMove(placementPos, dangerTile))
						break;

					ownDangerZone.push_back(dangerTile);

					if (enemy->QueryHasBoxAt(dangerTile))
						break;
				}
			}

			auto isInOwnDanger = [&](const Vector2& pos) -> bool {
				for (const Vector2& danger : ownDangerZone)
				{
					if (pos == danger)
						return true;
				}
				return false;
				};

			// Find shortest path to safety
			int minSteps = UNREACHABLE_STEPS;
			for (const Vector2& escapeDir : dirs)
			{
				int steps = 1;
				Vector2 checkPos = placementPos + escapeDir * ts;

				while (steps < MAX_SEARCH_STEPS)
				{
					if (!enemy->QueryCanMove(placementPos, checkPos))
						break;

					if (!isInOwnDanger(checkPos))
					{
						if (steps < minSteps)
							minSteps = steps;
						break;
					}

					checkPos = checkPos + escapeDir * ts;
					steps++;
				}
			}

			return minSteps;
		}

		// Evaluate if immediate escape is needed after placing water balloon
		// Returns true if escapePath should be kept (immediate escape), false if should be cleared (safe to wait)
		static bool ShouldKeepEscapePath(
			Enemy* enemy,
			const Vector2& placementPos,
			int ts)
		{
			auto stats = enemy->GetComponent<StatsComponent>();
			if (!stats) return false;

			int range = stats->GetExplosionRange();
			auto movementComp = enemy->GetComponent<MovementComponent>();
			float moveSpeed = movementComp ? movementComp->GetMoveSpeed() : MovementComponent::SPEED_NORMAL;

			int minStepsToSafety = FindMinStepsToSafety(enemy, placementPos, ts, range);
			float timeToEscape = minStepsToSafety * moveSpeed;

			// Need immediate escape if not enough time or too close
			if (timeToEscape >= WATER_BALLOON_EXPLOSION_TIME - IMMEDIATE_ESCAPE_TIME_THRESHOLD ||
				minStepsToSafety <= IMMEDIATE_ESCAPE_STEP_THRESHOLD)
			{
				return true; // Keep path for immediate escape
			}

			return false; // Safe to wait, clear path
		}
	};
}
