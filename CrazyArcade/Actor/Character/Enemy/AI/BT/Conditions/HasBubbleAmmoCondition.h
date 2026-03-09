#pragma once

#include "AI/BTNode.h"

class StatsComponent;

namespace enemy_ai
{
	class HasBubbleAmmoCondition : public engine::BTNode
	{
	public:
		explicit HasBubbleAmmoCondition(StatsComponent* stats)
			: stats(stats)
		{
		}

		engine::BTStatus Execute(float) override
		{
			if (stats && stats->GetBubbleAmmo() > 0)
				return engine::BTStatus::Success;
			return engine::BTStatus::Failure;
		}

	private:
		StatsComponent* stats;
	};
}
