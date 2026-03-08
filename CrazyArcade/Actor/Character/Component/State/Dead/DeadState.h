#pragma once

#include "../IState.h"

class DeadState : public IState
{
public:
	virtual void OnEnter(StateComponent* stateComp) override;
	virtual void OnExit(StateComponent* stateComp) override;
	virtual void Tick(StateComponent* stateComp, float deltaTime) override;

private:
	float blinkElapsed = 0.0f;
	bool blinkOn = false;
	static constexpr float BLINK_INTERVAL = 0.12f;
};
