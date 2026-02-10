#pragma once

#include "Actor/Actor.h"

using namespace engine;

class IGameRuleManager;

enum class ItemType // todo: bit map?
{
	BubbleUpgrade,
	Roller,
	Shield
};

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

	IGameRuleManager* gameRuleManager = nullptr;
};