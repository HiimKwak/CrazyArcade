#pragma once

#include "Actor/IComponent.h"
#include "../Character.h"

using namespace engine;

class ICharacterComponent : public IComponent
{
public:
	Character* GetCharacter() const { return static_cast<Character*>(owner); }
};
