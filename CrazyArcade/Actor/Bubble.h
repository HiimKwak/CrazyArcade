#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"

using namespace engine;

class ExplosionTilePool;

class Bubble : public Actor
{
	RTTI_DECLARATIONS(Bubble, Actor)

public:
	Bubble(const Vector2& newPosition, int explosionRange = 1);

	virtual void Tick(float deltaTime) override;

	bool IsExploded() const;

	void SetExplosionTilePool(ExplosionTilePool* pool);

private:
	void Explode(float deltaTime);

	Timer explodeTimer;
	float countDown = 3.0f;
	bool exploded = false;
	int range;
	ExplosionTilePool* segmentPool;
};

