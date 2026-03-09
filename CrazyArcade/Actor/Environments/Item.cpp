#include "Item.h"
#include "Actor/Character/Player/Player.h"
#include "../Effects/ExplosionTile.h"
#include "Level/Level.h"
#include "Util/Util.h"
#include "Interface/IWorldQueryProvider.h"
#include "Interface/IWorldQueryService.h"

Item::Item(const Vector2& position)
	: super(L"?", position, Color::White)
{
	sortingOrder = 3;
	itemType = static_cast<ItemType>(Util::Random(0, 2));

	switch (itemType)
	{
	case ItemType::BubbleUpgrade:
		SetSprite(L"��", Color::Blue);
		break;
	case ItemType::Roller:
		SetSprite(L"��", Color::Orange);
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
		SetSprite(L"��", Color::Blue);
		break;
	case ItemType::Roller:
		SetSprite(L"��", Color::Orange);
		break;
	case ItemType::Shield:
		SetSprite(L"S", Color::Skyblue);
		break;
	}
}

void Item::BeginPlay()
{
	super::BeginPlay();
	IWorldQueryProvider* provider = dynamic_cast<IWorldQueryProvider*>(GetOwner());
	worldQueryService = provider ? provider->GetWorldQueryService() : nullptr;
}

void Item::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!owner || !worldQueryService)
		return;

	Vector2 position = GetPosition();

	if (worldQueryService->HasExplosionAt(position))
	{
		Destroy();
		return;
	}

	if (worldQueryService->HasActorAt(position, ActorType::Character))
	{
		worldQueryService->SendItemToCharacter(position, itemType);
		Destroy();
		return;
	}
}