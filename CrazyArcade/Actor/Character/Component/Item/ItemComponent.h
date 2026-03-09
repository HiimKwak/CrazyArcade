#pragma once

#include <vector>
#include <memory>
#include <map>

#include "Actor/IComponent.h"
#include "ItemType.h"

using namespace engine;

class IUsableItem;
class IEffect;

class ItemComponent : public IComponent
{
public:
	ItemComponent();
	~ItemComponent();

	virtual void Tick(float deltaTime) override;

	void OnItemAcquired(ItemType type);
	bool RequestUseItem(ItemType type);

private:
	std::unique_ptr<IUsableItem> CreateUsableItem(ItemType type);
	void ApplyPassiveItemEffect(ItemType type);

	std::map<ItemType, std::unique_ptr<IUsableItem>> usableItems;
	std::vector<std::unique_ptr<IEffect>> activeEffects;
};
