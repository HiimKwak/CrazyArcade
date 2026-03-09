#include <Windows.h>

#include "Player.h"
#include "PlayerController.h"
#include "../Component/StatsComponent.h"
#include "../Component/InventoryComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/BubbleComponent.h"
#include "../Component/Item/ItemComponent.h"
#include "../Component/State/StateComponent.h"
#include "../MoveSpeed.h"

using namespace engine;

Player::Player(const Vector2& position)
	: Character(L"P", position, Color::Green)
{
	sortingOrder = 8;
	auto playerCtrl = CreateController<PlayerController>(this);
	playerCtrl->BindItemToKey('S', ItemType::Shield);

	auto stats = AddComponent<StatsComponent>();
	stats->SetLives(2);
	stats->SetMaxBubbleAmmo(2);
	stats->SetBubbleAmmo(2);
	stats->SetBubbleRange(1);

	AddComponent<InventoryComponent>();
	AddComponent<ItemComponent>();
	AddComponent<MovementComponent>();
	AddComponent<StateComponent>();
	AddComponent<BubbleComponent>();
}

void Player::BeginPlay()
{
	super::BeginPlay();
	auto movementComp = GetComponent<MovementComponent>();
	if (movementComp)
		movementComp->SetMoveSpeed(MoveSpeed::NORMAL);
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	auto stateComp = GetComponent<StateComponent>();
	if (stateComp && !stateComp->IsDead())
		controller->Tick(deltaTime);
}

void Player::OnDamaged()
{
	auto stats = GetComponent<StatsComponent>();
	if (stats)
	{
		stats->SetLives(stats->GetLives() - 1);
		if (stats->GetLives() <= 0)
		{
			auto stateComp = GetComponent<StateComponent>();
			if (stateComp)
				stateComp->ChangeState(StateType::Dead);
		}
	}
}

void Player::OnKilled()
{
	auto stateComp = GetComponent<StateComponent>();
	if (stateComp)
		stateComp->ChangeState(StateType::Dead);
}
