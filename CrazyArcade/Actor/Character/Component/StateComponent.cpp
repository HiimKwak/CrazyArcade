#include "StateComponent.h"
#include "../Character.h"

StateComponent::StateComponent()
{
	stateNormal = std::make_unique<NormalState>();
	stateBubbleTrapped = std::make_unique<BubbleTrappedState>();
	stateDead = std::make_unique<DeadState>();
}

void StateComponent::Initialize(Actor* inOwner)
{
	if (initialized)
		return;
	initialized = true;
	ICharacterComponent::Initialize(inOwner);
	if (stateNormal)
		stateNormal->OnEnter(this);
}

void StateComponent::Tick(float deltaTime)
{
	ICharacterState* statePtr = GetStateObject(currentState);
	if (statePtr)
		statePtr->Tick(this, deltaTime);
}

void StateComponent::ChangeState(ECharacterState newState)
{
	if (currentState == newState)
		return;

	ICharacterState* currentStatePtr = GetStateObject(currentState);
	if (currentStatePtr)
		currentStatePtr->OnExit(this);

	currentState = newState;

	ICharacterState* newStatePtr = GetStateObject(newState);

	if (GetCharacter())
	{
		switch (newState)
		{
		case ECharacterState::Normal:
			GetCharacter()->NotifyNormal();
			break;
		case ECharacterState::BubbleTrapped:
			GetCharacter()->NotifyBubbleTrapped();
			break;
		case ECharacterState::Dead:
			GetCharacter()->NotifyDied();
			break;
		}
	}

	if (newStatePtr)
		newStatePtr->OnEnter(this);
}

bool StateComponent::CanMove() const
{
	return currentState == ECharacterState::Normal || currentState == ECharacterState::BubbleTrapped;
}

bool StateComponent::CanAct() const
{
	return currentState == ECharacterState::Normal;
}

ICharacterState* StateComponent::GetStateObject(ECharacterState state)
{
	switch (state)
	{
	case ECharacterState::Normal:
		return stateNormal.get();
	case ECharacterState::BubbleTrapped:
		return stateBubbleTrapped.get();
	case ECharacterState::Dead:
		return stateDead.get();
	}
	return nullptr;
}
