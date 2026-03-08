#pragma once

namespace engine
{
	enum class BTStatus { Success, Failure, Running };

	class BTNode
	{
	public:
		virtual ~BTNode() = default;
		virtual BTStatus Execute(float deltaTime) = 0;
	};
}
