#pragma once

#include "Actor/Actor.h"

using namespace engine;

class Bubble : public Actor
{
	RTTI_DECLARATIONS(Bubble, Actor)

public:
	Bubble(const Vector2& newPosition);

	virtual void Tick(float deltaTime) override;

	bool IsExploded() const;
	bool IsInExplosionRange(const Actor* other) const;

private:
	void Explode();

	float elapsedTime = 0.0f;
	float countDown = 3.0f;
	bool exploded = false;
};

