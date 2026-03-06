#pragma once

#include <memory>

class Character;
class IItemEffect;

class IUsableItem
{
public:
	virtual ~IUsableItem() = default;

	virtual bool CanUse(Character* owner) const = 0;
	virtual std::unique_ptr<IItemEffect> Use(Character* owner) = 0;
};
