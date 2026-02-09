#pragma once

#include "Actor/Actor.h"

using namespace engine;

class Box : public Actor
{
	RTTI_DECLARATIONS(Box, Actor)

public:
	Box(const Vector2& position);

	void OnDamaged();

private:
	bool damaged = false;
};