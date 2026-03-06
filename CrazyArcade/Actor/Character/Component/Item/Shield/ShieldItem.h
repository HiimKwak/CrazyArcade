#pragma once

#include "../IUsableItem.h"

class ShieldItem : public IUsableItem
{
public:
	bool CanUse(Actor* owner) const override;
	std::unique_ptr<IEffect> Use(Actor* owner) override;
};
