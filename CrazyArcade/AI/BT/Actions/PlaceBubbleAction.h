#pragma once

#include "AI/BTNode.h"
#include "Actor/Character/Component/BubbleComponent.h"

class PlaceBubbleAction : public engine::BTNode
{
public:
	explicit PlaceBubbleAction(BubbleComponent* bubble)
		: bubble(bubble)
	{
	}

	engine::BTStatus Execute(float) override
	{
		if (!bubble)
			return engine::BTStatus::Failure;

		return bubble->RequestGenerateBubble()
			? engine::BTStatus::Success
			: engine::BTStatus::Failure;
	}

private:
	BubbleComponent* bubble;
};
