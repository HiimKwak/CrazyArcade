#pragma once

#include "Common/Common.h"

namespace engine
{
	class ENGINE_API IController
	{
	public:
		virtual ~IController() = default;

		virtual void Tick(float deltaTime) = 0;
	};
}
