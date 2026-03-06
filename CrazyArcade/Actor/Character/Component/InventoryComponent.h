#pragma once

#include <map>

#include "Actor/IComponent.h"
#include "Item/ItemType.h"

using namespace engine;

class InventoryComponent : public IComponent
{
public:
	virtual void Tick(float deltaTime) override;

	void AddItem(ItemType type, int count = 1);
	bool RemoveItem(ItemType type, int count = 1);
	bool HasItem(ItemType type) const;
	int GetItemCount(ItemType type) const;

private:
	std::map<ItemType, int> itemCounts;
};
