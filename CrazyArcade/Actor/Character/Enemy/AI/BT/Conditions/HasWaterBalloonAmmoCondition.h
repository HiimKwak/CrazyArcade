#pragma once

#include "AI/BTNode.h"

class StatsComponent;

namespace enemy_ai
{
	class HasWaterBalloonAmmoCondition : public engine::BTNode
	{
	public:
		explicit HasWaterBalloonAmmoCondition(StatsComponent* stats)
			: stats(stats)
		{
		}

		engine::BTStatus Execute(float) override
		{
			if (stats && stats->GetWaterBalloonAmmo() > 0)
				return engine::BTStatus::Success;
			return engine::BTStatus::Failure;
		}

	private:
		StatsComponent* stats;
	};
}
