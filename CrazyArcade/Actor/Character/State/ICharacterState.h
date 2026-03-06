#pragma once

#include "Enum.h"

class StateComponent;

class ICharacterState
{
public:
	virtual ~ICharacterState() = default;

	virtual void OnEnter(StateComponent* stateComp) = 0;
	virtual void OnExit(StateComponent* stateComp) = 0;
	virtual void Tick(StateComponent* stateComp, float deltaTime) = 0;
};
