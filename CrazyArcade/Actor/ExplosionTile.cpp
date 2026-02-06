#include "ExplosionTile.h"
#include "ExplosionTilePool.h"

ExplosionTile::ExplosionTile(const Vector2& tilePos, float lifetime)
	: super("#", tilePos, Color::Blue),
	lifetime(lifetime),
	active(false),
	pool(nullptr)
{
	sortingOrder = 6;
	lifeTimer.SetTargetTime(lifetime);
}

void ExplosionTile::Tick(float deltaTime)
{
	if (!active) return;

	super::Tick(deltaTime);

	lifeTimer.Tick(deltaTime);
	if (lifeTimer.IsTimeout())
	{
		if (pool)
			pool->Release(this);
		else
			Destroy();
	}
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

void ExplosionTile::SetPool(ExplosionTilePool* ownerPool)
{
	pool = ownerPool;
}
