#include "Box.h"
#include "Item.h"
#include "../Effects/ExplosionTile.h"
#include "Util/Util.h"
#include "Level/GameLevel.h"

Box::Box(const Vector2& position)
	: super(L"��", position, Color::Yellow)
{
	sortingOrder = 4;
}

void Box::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (damaged)
	{
		destroyTimer.Tick(deltaTime);
		if (destroyTimer.IsTimeout())
			SpawnItem();
	}
}

void Box::OnDamaged()
{
	if (damaged)
		return;

	damaged = true;

	destroyTimer.SetTargetTime(ExplosionTile::lifetime);
	destroyTimer.Reset();
}

void Box::SpawnItem()
{
	constexpr int itemSpawnDenominator = 4; // 25%
	spawnItem = (Util::Random(0, itemSpawnDenominator - 1) == 0);
	if (owner && spawnItem)
		owner->AddNewActor(new Item(GetPosition()));

	Destroy();
	spawnItem = false;
}