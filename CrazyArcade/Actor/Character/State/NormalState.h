#pragma once

#include "ICharacterState.h"

class NormalState : public ICharacterState
{
public:
	virtual void OnEnter(StateComponent* stateComp) override;
	virtual void OnExit(StateComponent* stateComp) override;
	virtual void Tick(StateComponent* stateComp, float deltaTime) override;
};
