#pragma once

enum class ItemType
{
	BubbleUpgrade,
	Roller,
	Shield
};

constexpr bool IsUsableItem(ItemType type)
{
	switch (type)
	{
	case ItemType::Shield:
		return true;

	default:
		return false;
	}
}