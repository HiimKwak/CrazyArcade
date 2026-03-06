#pragma once

class Character;

class IItemEffect
{
public:
	virtual ~IItemEffect() = default;

	virtual void Apply(Character* target) = 0;
	virtual void Tick(float deltaTime) = 0;
	virtual bool IsActive() const = 0;
};
