#pragma once

#include <vector>
#include <memory>
#include <map>
#include <functional>

#include "Actor/IComponent.h"
#include "ItemType.h"

using namespace engine;

class IUsableItem;
class IEffect;

class ItemComponent : public IComponent
{
public:
	using ItemAcquiredCallback = std::function<void(ItemType)>;

	ItemComponent();
	~ItemComponent();

	virtual void Tick(float deltaTime) override;

	void OnItemAcquired(ItemType type);
	void SetOnItemAcquiredCallback(ItemAcquiredCallback callback) { onItemAcquired = std::move(callback); }
	bool RequestUseItem(ItemType type);
	bool HasActiveShield() const;

	const std::map<ItemType, std::unique_ptr<IUsableItem>>& GetUsableItems() const { return usableItems; }
	const std::map<ItemType, int>& GetAcquiredItems() const { return acquiredItems; }

private:
	std::unique_ptr<IUsableItem> CreateUsableItem(ItemType type);
	void ApplyPassiveItemEffect(ItemType type);

	std::map<ItemType, std::unique_ptr<IUsableItem>> usableItems;
	std::vector<std::unique_ptr<IEffect>> activeEffects;
	std::map<ItemType, int> acquiredItems;
	ItemAcquiredCallback onItemAcquired;
};
