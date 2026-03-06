#pragma once

#include "../IState.h"

class BubbleTrappedState : public IState
{
public:
	virtual void OnEnter(StateComponent* stateComp) override;
	virtual void OnExit(StateComponent* stateComp) override;
	virtual void Tick(StateComponent* stateComp, float deltaTime) override;

private:
	float trapTimer = 0.0f;
	static constexpr float TRAP_DURATION = 3.0f;
};
