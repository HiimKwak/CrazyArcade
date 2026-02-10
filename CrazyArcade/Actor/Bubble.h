#pragma once

#include "Util/Timer.h"
#include "Actor/CharacterBase.h"

using namespace engine;

class Player;

class Bubble : public Actor
{
	RTTI_DECLARATIONS(Bubble, Actor)

public:
	Bubble(const Vector2& newPosition, int explosionRange = 1);

	virtual void Tick(float deltaTime) override;

	bool IsExploded() const;

	inline CharacterBase* GetOwnerCharacter() const { return ownerCharacter; }
	void SetOwnerCharacter(CharacterBase* actor) { ownerCharacter = actor; }

private:
	void Explode(float deltaTime);

	CharacterBase* ownerCharacter = nullptr;

	Timer explodeTimer;
	float countDown = 3.0f;
	bool exploded = false;
	int range;
};

