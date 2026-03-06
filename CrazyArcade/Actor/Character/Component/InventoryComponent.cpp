#include "InventoryComponent.h"

void InventoryComponent::Tick(float deltaTime)
{
}

void InventoryComponent::AddItem(ItemType type, int count)
{
	itemCounts[type] += count;
}

bool InventoryComponent::RemoveItem(ItemType type, int count)
{
	auto it = itemCounts.find(type);
	if (it == itemCounts.end() || it->second < count)
		return false;

	it->second -= count;
	if (it->second <= 0)
		itemCounts.erase(it);

	return true;
}

bool InventoryComponent::HasItem(ItemType type) const
{
	auto it = itemCounts.find(type);
	return it != itemCounts.end() && it->second > 0;
}

int InventoryComponent::GetItemCount(ItemType type) const
{
	auto it = itemCounts.find(type);
	return (it != itemCounts.end()) ? it->second : 0;
}
