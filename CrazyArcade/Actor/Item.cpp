#include "Item.h"
#include "Player.h"
#include "ExplosionTile.h"
#include "Level/Level.h"
#include "Interface/IGameRuleManager.h"

Item::Item(const Vector2& position, ItemType type)
	: super("?", position, Color::White),
	itemType(type)
{
	sortingOrder = 4;

	switch (type)
	{
	case ItemType::BubbleUpgrade:
		SetSprite("B", Color::Blue);
		break;
	case ItemType::Roller:
		SetSprite("R", Color::Orange);
		break;
	case ItemType::Shield:
		SetSprite("S", Color::Skyblue);
		break;
	}
}

void Item::BeginPlay()
{
	super::BeginPlay();
	gameRuleManager = dynamic_cast<IGameRuleManager*>(GetOwner());
}

void Item::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!owner)
		return;

	Vector2 position = GetPosition();

	if (gameRuleManager->HasExplosionAt(position))
	{
		Destroy();
		return;
	}
		
	if (gameRuleManager->HasPlayerAt(position))
	{
		gameRuleManager->SendItemToPlayer(position, itemType);
		Destroy();
		return;
	}
}