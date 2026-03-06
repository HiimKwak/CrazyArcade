#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"

using namespace engine;

class Box : public Actor
{
	RTTI_DECLARATIONS(Box, Actor)

public:
	Box(const Vector2& position);

	virtual void Tick(float deltaTime) override;

	void OnDamaged();

private:
	void SpawnItem();

private:
	bool damaged = false;
	bool spawnItem = false;
	Timer destroyTimer;
};