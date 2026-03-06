#pragma once

#include <memory>

#include "ICharacterComponent.h"
#include "../State/CharacterState.h"

class StateComponent : public ICharacterComponent
{
public:
	StateComponent();

	virtual void Initialize(Character* owner) override;
	virtual void Tick(float deltaTime) override;

	void ChangeState(ECharacterState newState);
	ECharacterState GetCurrentState() const { return currentState; }

	bool IsNormal() const { return currentState == ECharacterState::Normal; }
	bool IsBubbleTrapped() const { return currentState == ECharacterState::BubbleTrapped; }
	bool IsDead() const { return currentState == ECharacterState::Dead; }

	bool CanMove() const;
	bool CanAct() const;

private:
	ICharacterState* GetStateObject(ECharacterState state);

	ECharacterState currentState = ECharacterState::Normal;
	bool initialized = false;

	std::unique_ptr<ICharacterState> stateNormal;
	std::unique_ptr<ICharacterState> stateBubbleTrapped;
	std::unique_ptr<ICharacterState> stateDead;
};
