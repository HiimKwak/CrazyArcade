#pragma once

#include "Common/Common.h"

namespace engine
{
	class Actor;

	class ENGINE_API IComponent
	{
	public:
		virtual ~IComponent() = default;

		virtual void Initialize(Actor* inOwner) { owner = inOwner; }
		virtual void Tick(float deltaTime) = 0;

	protected:
		Actor* owner = nullptr;
	};
}
