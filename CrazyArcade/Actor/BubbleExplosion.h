#pragma once

#include "Actor/Actor.h"
#include <vector>

using namespace engine;

class ExplosionTile;
class ExplosionTilePool;

class BubbleExplosion : public Actor
{
	RTTI_DECLARATIONS(BubbleExplosion, Actor)

public:
	BubbleExplosion(const Vector2& origin, int explosionRange = 1, ExplosionTilePool* pool);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	void Propagate();
	void PropagateInDirection(const Vector2& direction, int range);

	int range = 1;
	ExplosionTilePool* segmentPool;
	std::vector<ExplosionTile*> segments;
	bool propagated;
};
