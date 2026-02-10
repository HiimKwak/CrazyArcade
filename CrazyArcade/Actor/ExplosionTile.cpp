#include "ExplosionTile.h"
#include "Level/GameLevel.h"

ExplosionTile::ExplosionTile(const Vector2& tilePos)
	: super(L"%", tilePos, Color::Skyblue),
	active(true)
{
	sortingOrder = 6;
	lifeTimer.SetTargetTime(lifetime);
}

void ExplosionTile::Draw()
{
	GameLevel* gameLevel = owner ? dynamic_cast<GameLevel*>(owner) : nullptr;

	if (gameLevel && gameLevel->IsInsideGameMap_Screen(GetPosition()))
		super::Draw();
}

void ExplosionTile::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!active) return;

	lifeTimer.Tick(deltaTime);
	if (lifeTimer.IsTimeout())
		Destroy();
}

void ExplosionTile::Reset(const Vector2& tilePos)
{
	SetPosition(tilePos);
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

