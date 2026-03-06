#pragma once

#include "StateType.h"

class StateComponent;

class IState
{
public:
	virtual ~IState() = default;

	virtual void OnEnter(StateComponent* stateComp) = 0;
	virtual void OnExit(StateComponent* stateComp) = 0;
	virtual void Tick(StateComponent* stateComp, float deltaTime) = 0;
};
