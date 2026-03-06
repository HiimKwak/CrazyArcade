#include "ShieldItem.h"
#include "ShieldEffect.h"

bool ShieldItem::CanUse(Actor* owner) const
{
	return true;
}

std::unique_ptr<IEffect> ShieldItem::Use(Actor* owner)
{
	auto effect = std::make_unique<ShieldEffect>();
	effect->Apply(owner);
	return effect;
}
