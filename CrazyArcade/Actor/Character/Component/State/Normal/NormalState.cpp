#include "NormalState.h"
#include "../StateComponent.h"
#include "../../MovementComponent.h"
#include "../../../Character.h"
#include "Actor/MoveSpeed.h"

void NormalState::OnEnter(StateComponent* stateComp)
{
	if (!stateComp) return;
	if (auto* character = stateComp->GetCharacter())
	{
		character->ResetSpriteColor();
		if (auto* movement = character->GetComponent<MovementComponent>())
			movement->SetMoveSpeed(MoveSpeed::NORMAL);
	}
}

void NormalState::OnExit(StateComponent* stateComp)
{
}

void NormalState::Tick(StateComponent* stateComp, float deltaTime)
{
}
