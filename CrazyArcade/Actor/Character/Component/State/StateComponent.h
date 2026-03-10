#pragma once

#include <memory>

#include "../ICharacterComponent.h"
#include "StateType.h"
#include "IState.h"

class StateComponent : public ICharacterComponent
{
public:
	StateComponent();

	virtual void Initialize(Actor* owner) override;
	virtual void Tick(float deltaTime) override;

	void ChangeState(StateType newState);
	StateType GetCurrentState() const { return currentState; }

	bool IsNormal() const { return currentState == StateType::Normal; }
	bool IsBubbled() const { return currentState == StateType::Bubbled; }
	bool IsDead() const { return currentState == StateType::Dead; }

	bool CanMove() const;
	bool CanAct() const;

private:
	IState* GetStateObject(StateType state);

	StateType currentState = StateType::Normal;
	bool initialized = false;

	std::unique_ptr<IState> stateNormal;
	std::unique_ptr<IState> stateBubbled;
	std::unique_ptr<IState> stateDead;
};
