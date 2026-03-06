#pragma once

#include "Actor/Actor.h"
#include <vector>

using namespace engine;

class ExplosionTile;

class BubbleExplosion : public Actor
{
	RTTI_DECLARATIONS(BubbleExplosion, Actor)

public:
	BubbleExplosion(const Vector2& origin, int explosionRange);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	void Propagate();
	void PropagateInDirection(const Vector2& direction, int range);

	int range = 1;
	bool propagated;
};
