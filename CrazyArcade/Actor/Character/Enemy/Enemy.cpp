#include <Windows.h>

#include "Enemy.h"
#include "EnemyController.h"
#include "../Component/CharacterComponent.h"
#include "Actor/MoveSpeed.h"

using namespace engine;

Enemy::Enemy(const Vector2& position)
	: Character(L"●", position, Color::Red)
{
	sortingOrder = 8;
	CreateController<EnemyController>(this);

	// Add components
	auto stats = AddComponent<StatsComponent>();
	stats->SetLives(1);
	stats->SetMaxBubbleAmmo(2);
	stats->SetBubbleAmmo(2);
	stats->SetBubbleRange(1);

	AddComponent<InventoryComponent>();
	AddComponent<MovementComponent>();
	AddComponent<StateComponent>();
	AddComponent<BubbleComponent>();
}

void Enemy::BeginPlay()
{
	super::BeginPlay();
	auto movementComp = GetComponent<MovementComponent>();
	if (movementComp)
	{
		movementComp->SetMoveSpeed(MoveSpeed::NORMAL);
	}
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	auto stateComp = GetComponent<StateComponent>();
	if (stateComp && !stateComp->IsDead())
	{
		controller->Tick(deltaTime);
	}
}

void Enemy::OnDamaged()
{
	auto stats = GetComponent<StatsComponent>();
	if (stats)
	{
		stats->SetLives(stats->GetLives() - 1);
		if (stats->GetLives() <= 0)
		{
			auto stateComp = GetComponent<StateComponent>();
			if (stateComp)
				stateComp->ChangeState(ECharacterState::Dead);
		}
	}
}
