#pragma once

#include "../IState.h"
#include "Actor/Character/Component/MovementComponent.h"

class BubbledState : public IState
{
public:
	virtual void OnEnter(StateComponent* stateComp) override;
	virtual void OnExit(StateComponent* stateComp) override;
	virtual void Tick(StateComponent* stateComp, float deltaTime) override;

private:
	float trapTimer = 0.0f;
	float previousMoveSpeed = MovementComponent::SPEED_NORMAL;
	bool hasStoredMoveSpeed = false;
	static constexpr float TRAP_DURATION = 3.0f;
};
