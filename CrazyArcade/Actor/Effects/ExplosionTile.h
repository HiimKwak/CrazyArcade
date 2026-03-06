#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"

using namespace engine;

class ExplosionTile : public Actor
{
	RTTI_DECLARATIONS(ExplosionTile, Actor)

public:
	static constexpr float lifetime = 0.5f;
	ExplosionTile(const Vector2& tilePos);

	virtual void Draw() override;
	virtual void Tick(float deltaTime) override;

	void Reset(const Vector2& tilePos);
	void Activate();
	void Deactivate();
	inline bool IsActive() const { return active; }

private:
	Timer lifeTimer;
	bool active;
};