#include "Bubble.h"

Bubble::Bubble(const Vector2& newPosition)
	:super("B", newPosition, Color::Blue)
{
	sortingOrder = 5;
}

void Bubble::Tick(float deltaTime)
{
	elapsedTime += deltaTime;

	if (!exploded && elapsedTime >= countDown)
		Explode();
}

bool Bubble::IsExploded() const
{
	return exploded;
}

bool Bubble::IsInExplosionRange(const Actor* other) const
{
	Vector2 myTile = GetPosition();
	Vector2 otherTile = other->GetPosition();

	Vector2 diff = otherTile - myTile;

	if (diff == Vector2(0, 0)) return true;
	if (diff == Vector2(1, 0)) return true;
	if (diff == Vector2(-1, 0)) return true;
	if (diff == Vector2(0, 1)) return true;
	if (diff == Vector2(0, -1)) return true;

	return false;
}

void Bubble::Explode()
{
	exploded = true;
}
