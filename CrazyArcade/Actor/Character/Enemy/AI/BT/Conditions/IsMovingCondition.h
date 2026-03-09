#pragma once

#include "AI/BTNode.h"

class MovementComponent;

namespace enemy_ai
{
	class IsMovingCondition : public engine::BTNode
	{
	public:
		explicit IsMovingCondition(MovementComponent* movement)
			: movement(movement)
		{
		}

		engine::BTStatus Execute(float) override
		{
			if (movement && movement->IsMoving())
				return engine::BTStatus::Success;
			return engine::BTStatus::Failure;
		}

	private:
		MovementComponent* movement;
	};
}
