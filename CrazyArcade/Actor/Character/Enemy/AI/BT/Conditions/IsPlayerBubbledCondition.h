#pragma once

#include "AI/BTNode.h"
#include "Actor/Character/Enemy/Enemy.h"

namespace enemy_ai
{
	class IsPlayerBubbledCondition : public engine::BTNode
	{
	public:
		explicit IsPlayerBubbledCondition(Enemy* enemy, bool expected = true)
			: enemy(enemy), expected(expected)
		{
		}

		engine::BTStatus Execute(float) override
		{
			if (!enemy) return engine::BTStatus::Failure;
			bool bubbled = enemy->QueryIsPlayerBubbled();
			return bubbled == expected ? engine::BTStatus::Success : engine::BTStatus::Failure;
		}

	private:
		Enemy* enemy;
		bool expected;
	};
}
