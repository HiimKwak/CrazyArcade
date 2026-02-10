#include "ExplosionTile.h"

ExplosionTile::ExplosionTile(const Vector2& tilePos, float lifetime)
	: super(L"#", tilePos, Color::Skyblue),
	lifetime(lifetime),
	active(true)
{
	sortingOrder = 6;
	lifeTimer.SetTargetTime(lifetime);
}

void ExplosionTile::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!active) return;

	lifeTimer.Tick(deltaTime);
	if (lifeTimer.IsTimeout())
		Destroy();
}

void ExplosionTile::Reset(const Vector2& tilePos, float newLifetime)
{
	SetPosition(tilePos);
	lifetime = newLifetime;
	lifeTimer.Reset();
	lifeTimer.SetTargetTime(lifetime);
	active = false;
}

void ExplosionTile::Activate()
{
	active = true;
}

void ExplosionTile::Deactivate()
{
	active = false;
	lifeTimer.Reset();
}

