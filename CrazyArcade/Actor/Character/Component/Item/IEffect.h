#pragma once

namespace engine { class Actor; }
using namespace engine;

class IEffect
{
public:
	virtual ~IEffect() = default;

	virtual void Apply(Actor* target) = 0;
	virtual void Tick(float deltaTime) = 0;
	virtual bool IsActive() const = 0;
};
