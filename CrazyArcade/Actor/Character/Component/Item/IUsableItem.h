#pragma once

#include <memory>

namespace engine { class Actor; }
using namespace engine;

class IEffect;

class IUsableItem
{
public:
	virtual ~IUsableItem() = default;

	virtual bool CanUse(Actor* owner) const = 0;
	virtual std::unique_ptr<IEffect> Use(Actor* owner) = 0;
};
