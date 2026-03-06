#include "BubbleTrappedState.h"
#include "../Component/StateComponent.h"

void BubbleTrappedState::OnEnter(StateComponent* stateComp)
{
	trapTimer = TRAP_DURATION;
}

void BubbleTrappedState::OnExit(StateComponent* stateComp)
{
	trapTimer = 0.0f;
}

void BubbleTrappedState::Tick(StateComponent* stateComp, float deltaTime)
{
	trapTimer -= deltaTime;
	if (trapTimer <= 0.0f)
	{
		stateComp->ChangeState(ECharacterState::Normal);
	}
}
