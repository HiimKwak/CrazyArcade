#pragma once

class Character;

class ICharacterComponent
{
public:
	virtual ~ICharacterComponent() = default;

	virtual void Initialize(Character* inOwner) { owner = inOwner; }
	virtual void Tick(float deltaTime) = 0;

protected:
	Character* owner = nullptr;
};
