#pragma once

#include "../IUsableItem.h"

class ShieldItem : public IUsableItem
{
public:
	bool CanUse(Character* owner) const override;
	std::unique_ptr<IItemEffect> Use(Character* owner) override;
};
