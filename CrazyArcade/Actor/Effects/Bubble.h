#pragma once

#include "Util/Timer.h"
#include "Actor/Character/Character.h"

using namespace engine;

class Bubble : public Actor
{
	RTTI_DECLARATIONS(Bubble, Actor)

public:
	Bubble(const Vector2& newPosition, int explosionRange = 1);

	virtual void Tick(float deltaTime) override;

	bool IsExploded() const;

	inline Character* GetOwnerCharacter() const { return ownerCharacter; }
	void SetOwnerCharacter(Character* actor) { ownerCharacter = actor; }

private:
	void Explode(float deltaTime);

	Character* ownerCharacter = nullptr;

	Timer explodeTimer;
	float countDown = 3.0f;
	bool exploded = false;
	int range;
};

