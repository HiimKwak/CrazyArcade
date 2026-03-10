#include "BubbledState.h"
#include "../StateComponent.h"
#include "../../MovementComponent.h"
#include "../../StatsComponent.h"
#include "../../../Character.h"
#include "../../../MoveSpeed.h"

void BubbledState::OnEnter(StateComponent* stateComp)
{
	trapTimer = TRAP_DURATION;

	if (!stateComp) return;
	if (auto* character = stateComp->GetCharacter())
	{
		character->SetSpriteColor(Color::Skyblue);
		if (auto* movement = character->GetComponent<MovementComponent>())
			movement->SetMoveSpeed(MoveSpeed::SLOW);
	}
}

void BubbledState::OnExit(StateComponent* stateComp)
{
	trapTimer = 0.0f;

	if (!stateComp) return;
	if (auto* character = stateComp->GetCharacter())
	{
		character->ResetSpriteColor();
		if (auto* movement = character->GetComponent<MovementComponent>())
			movement->SetMoveSpeed(MoveSpeed::NORMAL);
	}
}

void BubbledState::Tick(StateComponent* stateComp, float deltaTime)
{
	if (!stateComp) return;

	trapTimer -= deltaTime;
	if (trapTimer > 0.0f)
		return;

	auto* character = stateComp->GetCharacter();
	if (!character)
		return;

	auto* stats = character->GetComponent<StatsComponent>();
	if (!stats)
	{
		stateComp->ChangeState(StateType::Normal);
		return;
	}

	stats->SetLives(stats->GetLives() - 1);
	if (stats->GetLives() <= 0)
		stateComp->ChangeState(StateType::Dead);
	else
		stateComp->ChangeState(StateType::Normal);
}
