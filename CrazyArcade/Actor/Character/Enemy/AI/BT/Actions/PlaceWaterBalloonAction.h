#pragma once

#include "AI/BTNode.h"
#include "Actor/Character/Component/WaterBalloonComponent.h"

namespace enemy_ai
{
	class PlaceWaterBalloonAction : public engine::BTNode
	{
	public:
		explicit PlaceWaterBalloonAction(WaterBalloonComponent* waterBalloon)
			: waterBalloon(waterBalloon)
		{
		}

		engine::BTStatus Execute(float) override
		{
			if (!waterBalloon)
				return engine::BTStatus::Failure;

			return waterBalloon->RequestGenerateWaterBalloon()
				? engine::BTStatus::Success
				: engine::BTStatus::Failure;
		}

	private:
		WaterBalloonComponent* waterBalloon;
	};
}
