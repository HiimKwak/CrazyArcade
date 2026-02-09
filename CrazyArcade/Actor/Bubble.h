#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"

using namespace engine;

class Player;

class Bubble : public Actor
{
	RTTI_DECLARATIONS(Bubble, Actor)

public:
	Bubble(const Vector2& newPosition, int explosionRange = 1);

	virtual void Tick(float deltaTime) override;

	bool IsExploded() const;

	inline Player* GetOwner() const { return ownerPlayer; }
	void SetOwnerPlayer(Player* player) { ownerPlayer = player; }

private:
	void Explode(float deltaTime);

	Player* ownerPlayer = nullptr;

	Timer explodeTimer;
	float countDown = 3.0f;
	bool exploded = false;
	int range;
};

