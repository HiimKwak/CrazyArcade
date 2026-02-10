#include <iostream>
#include <Windows.h>

#include "Enemy.h"
#include "AI/EnemyBrain.h"

#include "Level/Level.h"
#include "Interface/IGameRuleManager.h"

#include "Actor/Bubble.h"
#include "Actor/Item.h"

#include "Util/Timer.h"
#include "Math/Vector2.h"

using namespace engine;

Enemy::Enemy(const Vector2& position)
	: super(L"Ψ", position, Color::Red)
{
	sortingOrder = 8;

	lives = 1;
	maxBubbleAmmo = 0; // 똑똑해질 때까지 압수
	bubbleAmmo = maxBubbleAmmo;
	bubbleRange = 1;

	brain = new EnemyBrain();
}

Enemy::~Enemy() = default;

void Enemy::ChangeState(EnemyState newState)
{
	if (currentState == newState)
		return;

	if (isMoving)
	{
		SetPosition(moveTargetPos);
		isMoving = false;
	}

	currentState = newState;

	switch (newState)
	{
	case EnemyState::NORMAL:
		OnEnterNormal();
		break;

	case EnemyState::TRAPPED_IN_BUBBLE:
		OnEnterBubbleTrap();
		break;

	case EnemyState::DEAD:
		OnEnterDead();
		break;
	}
}

void Enemy::UpdateStateTick(float deltaTime)
{
	switch (currentState)
	{
	case EnemyState::DEAD:
		UpdateDeadTick(deltaTime);
		break;

	case EnemyState::TRAPPED_IN_BUBBLE:
		UpdateBubbleTrapTick(deltaTime);
		break;

	case EnemyState::NORMAL:
		UpdateNormalTick(deltaTime);
		break;
	}
}

void Enemy::UpdateDeadTick(float deltaTime)
{
}

void Enemy::UpdateBubbleTrapTick(float deltaTime)
{
	bubbleTrapTimer.Tick(deltaTime);

	if (!bubbleTrapTimer.IsTimeout())
		return;

	lives--;
	ChangeState(EnemyState::NORMAL);
}

void Enemy::UpdateNormalTick(float deltaTime)
{
}

void Enemy::HandleMovement(float deltaTime)
{
	if (isMoving)
	{
		TickMovementInterpolation(deltaTime);
		return;
	}

	thinkTimer.Tick(deltaTime);
	if (!thinkTimer.IsTimeout())
		return;

	thinkTimer.Reset();
	thinkTimer.SetTargetTime(Util::RandomRange(0.45f, 1.25f));

	Vector2 playerPos = gameRuleManager->GetPlayerPosition();
	desiredDirection = brain ? brain->ChooseDirection(GetPosition(), playerPos, gameRuleManager) : Vector2::Zero;

	TryStartMove(desiredDirection);
}

void Enemy::HandleAction(float deltaTime)
{
	shootTimer.Tick(deltaTime);
	if (!shootTimer.IsTimeout())
		return;

	if (!gameRuleManager->HasBubbleAt(GetPosition()) && bubbleAmmo > 0)
	{
		bubbleAmmo--;
		Bubble* bubble = new Bubble(GetPosition(), bubbleRange);
		bubble->SetOwnerCharacter(this);
		GetOwner()->AddNewActor(bubble);
	}

	shootTimer.Reset();
	shootTimer.SetTargetTime(Util::RandomRange(3.0f, 6.0f));
}

void Enemy::TryStartMove(const Vector2& direction)
{
	if (direction == Vector2::Zero)
		return;

	Vector2 targetPosition = GetPosition() + direction;
	if (!gameRuleManager->CanMove(GetPosition(), targetPosition))
		return;

	moveStartPos = GetPosition();
	moveTargetPos = targetPosition;
	isMoving = true;
	moveProgress = 0.0f;

	moveTimer.Reset();
	moveTimer.SetTargetTime(moveSpeed);
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!gameRuleManager)
		return;

	UpdateStateTick(deltaTime);

	if (currentState == EnemyState::DEAD)
		return;

	if (CanMove())
		HandleMovement(deltaTime);

	if (CanAct())
		HandleAction(deltaTime);
}

void Enemy::Draw()
{
	super::Draw();
}

void Enemy::OnEnterNormal()
{
	moveSpeed = MoveSpeed::NORMAL;
	SetSprite(L"Ψ", Color::Red);
}

void Enemy::OnEnterBubbleTrap()
{
	moveSpeed = MoveSpeed::SLOW;
	bubbleTrapTimer.Reset();
	bubbleTrapTimer.SetTargetTime(bubbleTrapDuration);
	SetSprite(L"0", Color::Orange);
}

void Enemy::OnEnterDead()
{
	SetSprite(L"X", Color::Red);
}

void Enemy::OnDamaged()
{
	if (UseShieldItem())
		return;

	if (currentState == EnemyState::NORMAL)
	{
		if (lives > 0)
			ChangeState(EnemyState::TRAPPED_IN_BUBBLE);
		else
			ChangeState(EnemyState::DEAD);
	}
	else if (currentState == EnemyState::TRAPPED_IN_BUBBLE)
		ChangeState(EnemyState::DEAD);
}
