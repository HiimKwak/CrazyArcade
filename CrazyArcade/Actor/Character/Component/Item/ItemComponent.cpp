#include "ItemComponent.h"
#include "../InventoryComponent.h"
#include "Actor/Actor.h"
#include "IUsableItem.h"
#include "IEffect.h"
#include "Shield/ShieldItem.h"

#include <algorithm>

ItemComponent::ItemComponent() = default;

ItemComponent::~ItemComponent() = default;

void ItemComponent::Tick(float deltaTime)
{
	for (auto& effect : activeEffects)
	{
		effect->Tick(deltaTime);
	}

	activeEffects.erase(
		std::remove_if(activeEffects.begin(), activeEffects.end(),
			[](const std::unique_ptr<IEffect>& e) { return !e->IsActive(); }),
		activeEffects.end());
}

std::unique_ptr<IUsableItem> ItemComponent::CreateUsableItem(ItemType type)
{
	switch (type)
	{
	case ItemType::Shield:
		return std::make_unique<ShieldItem>();
	default:
		return nullptr;
	}
}

void ItemComponent::OnItemAcquired(ItemType type)
{
	if (usableItems.find(type) == usableItems.end())
	{
		auto usable = CreateUsableItem(type);
		if (usable)
			usableItems[type] = std::move(usable);
	}

	auto inventory = owner ? owner->GetComponent<InventoryComponent>() : nullptr;
	if (inventory)
		inventory->AddItem(type);
}

bool ItemComponent::RequestUseItem(ItemType type)
{
	if (!owner)
		return false;

	auto inventory = owner->GetComponent<InventoryComponent>();
	if (!inventory || !inventory->HasItem(type))
		return false;

	auto it = usableItems.find(type);
	if (it == usableItems.end())
		return false;

	if (!it->second->CanUse(owner))
		return false;

	auto effect = it->second->Use(owner);
	inventory->RemoveItem(type);

	if (effect)
	{
		activeEffects.push_back(std::move(effect));
	}

	return true;
}
