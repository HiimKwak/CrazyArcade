#pragma once

#include "BTNode.h"
#include <memory>
#include <vector>

namespace engine
{
	class BTComposite : public BTNode
	{
	public:
		void AddChild(std::unique_ptr<BTNode> child)
		{
			children.push_back(std::move(child));
		}

	protected:
		std::vector<std::unique_ptr<BTNode>> children;
	};

	// 모든 자식이 Success여야 Success 반환
	class BTSequence : public BTComposite
	{
	public:
		BTStatus Execute(float deltaTime) override
		{
			for (auto& child : children)
			{
				BTStatus s = child->Execute(deltaTime);
				if (s != BTStatus::Success)
					return s;
			}
			return BTStatus::Success;
		}
	};

	// 첫 번째 Success에서 즉시 반환
	class BTSelector : public BTComposite
	{
	public:
		BTStatus Execute(float deltaTime) override
		{
			for (auto& child : children)
			{
				BTStatus s = child->Execute(deltaTime);
				if (s != BTStatus::Failure)
					return s;
			}
			return BTStatus::Failure;
		}
	};
}
