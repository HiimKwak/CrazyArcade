#include <Windows.h>

#include "Enemy.h"
#include "EnemyController.h"
#include "../Component/StatsComponent.h"
#include "../Component/InventoryComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/WaterBalloonComponent.h"
#include "../Component/Item/ItemComponent.h"
#include "../Component/State/StateComponent.h"
#include "../MoveSpeed.h"

using namespace engine;

Enemy::Enemy(const Vector2& position)
	: Character(L"E", position, Color::Red)
{
	sortingOrder = 8;

	auto stats = AddComponent<StatsComponent>();
	stats->SetLives(1);
	stats->SetMaxWaterBalloonAmmo(2);
	stats->SetWaterBalloonAmmo(2);
	stats->SetExplosionRange(1);

	AddComponent<InventoryComponent>();
	AddComponent<ItemComponent>();
	AddComponent<MovementComponent>();
	AddComponent<StateComponent>();
	AddComponent<WaterBalloonComponent>();

	// 컴포넌트 추가 후 생성 (컨트롤러 생성자에서 컴포넌트 접근)
	CreateController<EnemyController>(this);
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
				stateComp->ChangeState(StateType::Dead);
		}
	}
}
