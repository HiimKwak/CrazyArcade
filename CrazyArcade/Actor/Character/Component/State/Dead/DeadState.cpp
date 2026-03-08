#include "DeadState.h"
#include "../StateComponent.h"
#include "../../../Character.h"

void DeadState::OnEnter(StateComponent* stateComp)
{
	blinkElapsed = 0.0f;
	blinkOn = false;

	if (!stateComp) return;
	if (auto* character = stateComp->GetCharacter())
		character->SetSpriteColor(Color::Red);
}

void DeadState::OnExit(StateComponent* stateComp)
{
	if (!stateComp) return;
	if (auto* character = stateComp->GetCharacter())
		character->ResetSpriteColor();
}

void DeadState::Tick(StateComponent* stateComp, float deltaTime)
{
	if (!stateComp) return;
	auto* character = stateComp->GetCharacter();
	if (!character) return;

	blinkElapsed += deltaTime;
	if (blinkElapsed >= BLINK_INTERVAL)
	{
		blinkElapsed = 0.0f;
		blinkOn = !blinkOn;
		character->SetSpriteColor(blinkOn ? Color::White : Color::Red);
	}
}
