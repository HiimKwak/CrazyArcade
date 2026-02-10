#include "Item.h"
#include "Player.h"
#include "ExplosionTile.h"
#include "Level/Level.h"
#include "Util/Util.h"
#include "Interface/IGameRuleManager.h"

Item::Item(const Vector2& position)
	: super(L"?", position, Color::White)
{
	sortingOrder = 3;
	itemType = static_cast<ItemType>(Util::Random(0, 2));

	switch (itemType)
	{
	case ItemType::BubbleUpgrade:
		SetSprite(L"¢Ä", Color::Blue);
		break;
	case ItemType::Roller:
		SetSprite(L"¢Ý", Color::Orange);
		break;
	case ItemType::Shield:
		SetSprite(L"S", Color::Skyblue);
		break;
	}
}

Item::Item(const Vector2& position, ItemType type)
	: super(L"?", position, Color::White),
	itemType(type)
{
	sortingOrder = 3;

	switch (type)
	{
	case ItemType::BubbleUpgrade:
		SetSprite(L"¢Ä", Color::Blue);
		break;
	case ItemType::Roller:
		SetSprite(L"¢Ý", Color::Orange);
		break;
	case ItemType::Shield:
		SetSprite(L"S", Color::Skyblue);
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