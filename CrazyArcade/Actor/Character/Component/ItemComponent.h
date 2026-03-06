#pragma once

#include <vector>
#include <memory>
#include <map>

#include "ICharacterComponent.h"
#include "Actor/Items/ItemType.h"

class IUsableItem;
class IItemEffect;

class ItemComponent : public ICharacterComponent
{
public:
	ItemComponent();
	~ItemComponent();

	virtual void Tick(float deltaTime) override;

	void OnItemAcquired(ItemType type);
	bool RequestUseItem(ItemType type);

private:
	std::unique_ptr<IUsableItem> CreateUsableItem(ItemType type);

	std::map<ItemType, std::unique_ptr<IUsableItem>> usableItems;
	std::vector<std::unique_ptr<IItemEffect>> activeEffects;
};
