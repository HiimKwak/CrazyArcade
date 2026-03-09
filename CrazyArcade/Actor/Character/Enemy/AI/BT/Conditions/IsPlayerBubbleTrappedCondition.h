#pragma once

#include "AI/BTNode.h"
#include "Actor/Character/Enemy/Enemy.h"

namespace enemy_ai
{
	class IsPlayerBubbleTrappedCondition : public engine::BTNode
	{
	public:
		explicit IsPlayerBubbleTrappedCondition(Enemy* enemy, bool expected = true)
			: enemy(enemy), expected(expected)
		{
		}

		engine::BTStatus Execute(float) override
		{
			if (!enemy) return engine::BTStatus::Failure;
			bool trapped = enemy->QueryIsPlayerBubbleTrapped();
			return trapped == expected ? engine::BTStatus::Success : engine::BTStatus::Failure;
		}

	private:
		Enemy* enemy;
		bool expected;
	};
}
