#include "ShieldItem.h"
#include "../Effects/ShieldEffect.h"

bool ShieldItem::CanUse(Character* owner) const
{
	return true;
}

std::unique_ptr<IItemEffect> ShieldItem::Use(Character* owner)
{
	auto effect = std::make_unique<ShieldEffect>();
	effect->Apply(owner);
	return effect;
}
