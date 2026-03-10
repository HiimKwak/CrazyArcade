#pragma once

#include "Actor/Actor.h"
#include <vector>

using namespace engine;

class ExplosionTile;

class WaterBalloonExplosion : public Actor
{
	RTTI_DECLARATIONS(WaterBalloonExplosion, Actor)

public:
	WaterBalloonExplosion(const Vector2& origin, int explosionRange);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	void Propagate();
	void PropagateInDirection(const Vector2& direction, int range);

	int range = 1;
	bool propagated;
};
