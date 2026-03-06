#pragma once
#include "Actor/Actor.h"

using namespace engine;

class Wall :
	public Actor
{
	RTTI_DECLARATIONS(Wall, Actor)

public:
	Wall(const Vector2& position);
	Wall(const Vector2& position, bool visible);

	virtual void Draw() override;

private:
	bool isVisible = true;
};

