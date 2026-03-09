#pragma once

#include "Actor/Actor.h"
#include "Actor/Character/Component/Item/ItemType.h"

using namespace engine;

class IWorldQueryService;

class Item : public Actor
{
	RTTI_DECLARATIONS(Item, Actor)

public:
	Item(const Vector2& position);
	Item(const Vector2& position, ItemType type);

	virtual void Tick(float deltaTime) override;

	inline ItemType GetItemType() const { return itemType; }

protected:
	virtual void BeginPlay() override;

private:
	ItemType itemType;

	IWorldQueryService* worldQueryService = nullptr;
};