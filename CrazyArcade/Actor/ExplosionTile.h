#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"

using namespace engine;

class ExplosionTilePool;

class ExplosionTile : public Actor
{
	RTTI_DECLARATIONS(ExplosionTile, Actor)

public:
	ExplosionTile(const Vector2& tilePos, float lifetime = 1.0f);

	virtual void Tick(float deltaTime) override;

	void Reset(const Vector2& tilePos, float lifetime);
	void Activate();
	void Deactivate();
	inline bool IsActive() const { return active; }

	void SetPool(ExplosionTilePool* ownerPool);

private:
	Timer lifeTimer;
	float lifetime;
	bool active;
	ExplosionTilePool* pool;
};
