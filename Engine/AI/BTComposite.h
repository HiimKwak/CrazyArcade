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

	// every descendant needs to return success(&& condition)
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

	// success of a single child is sufficient(|| condition)
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
