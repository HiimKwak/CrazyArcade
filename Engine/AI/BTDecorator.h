#pragma once

#include "BTNode.h"
#include <memory>

namespace engine
{
	class BTDecorator : public BTNode
	{
	public:
		void SetChild(std::unique_ptr<BTNode> node) { child = std::move(node); }

	protected:
		std::unique_ptr<BTNode> child;
	};

	// 자식 결과를 반전
	class BTInverter : public BTDecorator
	{
	public:
		BTStatus Execute(float deltaTime) override
		{
			if (!child)
				return BTStatus::Failure;

			BTStatus s = child->Execute(deltaTime);
			if (s == BTStatus::Success) return BTStatus::Failure;
			if (s == BTStatus::Failure) return BTStatus::Success;
			return BTStatus::Running;
		}
	};

	// 쿨다운 동안 Failure 반환, 쿨다운 만료 후 자식 실행
	class BTCooldown : public BTDecorator
	{
	public:
		explicit BTCooldown(float cooldownTime)
			: cooldownTime(cooldownTime), elapsed(0.0f)
		{
		}

		BTStatus Execute(float deltaTime) override
		{
			elapsed += deltaTime;
			if (elapsed < cooldownTime)
				return BTStatus::Failure;

			if (!child)
				return BTStatus::Failure;

			BTStatus s = child->Execute(deltaTime);
			if (s != BTStatus::Running)
				elapsed = 0.0f;

			return s;
		}

	private:
		float cooldownTime;
		float elapsed;
	};
}
