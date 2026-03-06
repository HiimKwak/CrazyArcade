#include "StateComponent.h"
#include "../../Character.h"
#include "Normal/NormalState.h"
#include "BubbleTrapped/BubbleTrappedState.h"
#include "Dead/DeadState.h"

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
	IState* statePtr = GetStateObject(currentState);
	if (statePtr)
		statePtr->Tick(this, deltaTime);
}

void StateComponent::ChangeState(StateType newState)
{
	if (currentState == newState)
		return;

	IState* currentStatePtr = GetStateObject(currentState);
	if (currentStatePtr)
		currentStatePtr->OnExit(this);

	currentState = newState;

	IState* newStatePtr = GetStateObject(newState);

	if (GetCharacter())
	{
		switch (newState)
		{
		case StateType::Normal:
			GetCharacter()->NotifyNormal();
			break;
		case StateType::BubbleTrapped:
			GetCharacter()->NotifyBubbleTrapped();
			break;
		case StateType::Dead:
			GetCharacter()->NotifyDied();
			break;
		}
	}

	if (newStatePtr)
		newStatePtr->OnEnter(this);
}

bool StateComponent::CanMove() const
{
	return currentState == StateType::Normal || currentState == StateType::BubbleTrapped;
}

bool StateComponent::CanAct() const
{
	return currentState == StateType::Normal;
}

IState* StateComponent::GetStateObject(StateType state)
{
	switch (state)
	{
	case StateType::Normal:
		return stateNormal.get();
	case StateType::BubbleTrapped:
		return stateBubbleTrapped.get();
	case StateType::Dead:
		return stateDead.get();
	}
	return nullptr;
}
