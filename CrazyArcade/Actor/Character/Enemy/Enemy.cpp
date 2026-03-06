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
	controller = new EnemyController(this);

	// Add components
	auto stats = AddComponent<StatsComponent>();
	stats->SetLives(1);
	stats->SetMaxBubbleAmmo(0); // 몬스터는 버블을 발사하지 않음
	stats->SetBubbleAmmo(0);
	stats->SetBubbleRange(1);

	AddComponent<InventoryComponent>();
	AddComponent<MovementComponent>();
	AddComponent<StateComponent>();
	AddComponent<BubbleComponent>();
}

Enemy::~Enemy()
{
	delete controller;
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
