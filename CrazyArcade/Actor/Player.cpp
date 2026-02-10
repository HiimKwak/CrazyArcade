#include <iostream>
#include <Windows.h>

#include "Player.h"
#include "Core/Input.h"

#include "Level/Level.h"
#include "Interface/IGameRuleManager.h"

#include "Bubble.h"
#include "Item.h"

#include "Util/Timer.h"
#include "Math/Vector2.h"

using namespace engine;

Player::Player(const Vector2& position)
	: super(L"P", position, Color::Green)
{
	sortingOrder = 8;

	lives = 2;
	maxBubbleAmmo = 3;
	bubbleAmmo = maxBubbleAmmo;
	bubbleRange = 1;
}

void Player::ChangeState(PlayerState newState)
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
	case PlayerState::NORMAL:
		OnEnterNormal();
		break;

	case PlayerState::TRAPPED_IN_BUBBLE:
		OnEnterBubbleTrap();
		break;

	case PlayerState::DEAD:
		OnEnterDead();
		break;
	}
}

void Player::UpdateStateTick(float deltaTime)
{
	switch (currentState)
	{
	case PlayerState::DEAD:
		UpdateDeadTick(deltaTime);
		break;

	case PlayerState::TRAPPED_IN_BUBBLE:
		UpdateBubbleTrapTick(deltaTime);
		break;

	case PlayerState::NORMAL:
		UpdateNormalTick(deltaTime);
		break;
	}
}

void Player::UpdateDeadTick(float deltaTime)
{
}

void Player::UpdateBubbleTrapTick(float deltaTime)
{
	bubbleTrapTimer.Tick(deltaTime);

	if (!bubbleTrapTimer.IsTimeout())
		return;

	lives--;
	ChangeState(PlayerState::NORMAL);
}

void Player::UpdateNormalTick(float deltaTime)
{
}

void Player::TryMove(Vector2& direction)
{
	if (isMoving)
		return;

	Vector2 targetPosition(GetPosition() + direction);

	if (!gameRuleManager->CanMove(GetPosition(), targetPosition))
		return;

	bool hasBubbleAtTarget = gameRuleManager->HasBubbleAt(targetPosition);
	if (currentState == PlayerState::NORMAL && hasBubbleAtTarget && !gameRuleManager->Push(GetPosition(), targetPosition))
		return;

	moveStartPos = GetPosition();
	moveTargetPos = targetPosition;
	isMoving = true;
	moveProgress = 0.0f;
	moveTimer.Reset();
	moveTimer.SetTargetTime(moveSpeed);
}

void Player::HandleMovementInput(float deltaTime)
{
	if (isMoving)
	{
		TickMovementInterpolation(deltaTime);
	}
	else
	{
		if (!gameRuleManager)
			return;

		if (Input::Get().GetKey(VK_RIGHT))
			TryMove(Vector2::Right);
		if (Input::Get().GetKey(VK_LEFT))
			TryMove(Vector2::Left);
		if (Input::Get().GetKey(VK_DOWN))
			TryMove(Vector2::Up);
		if (Input::Get().GetKey(VK_UP))
			TryMove(Vector2::Down);
	}
}

void Player::HandleActionInput()
{
	if (!gameRuleManager)
		return;

	if (Input::Get().GetKey((VK_SPACE)) && 0 < bubbleAmmo)
	{
		if (gameRuleManager->HasBubbleAt(GetPosition()))
			return;

		bubbleAmmo--;
		Bubble* bubble = new Bubble(GetPosition(), bubbleRange);
		bubble->SetOwnerCharacter(this);

		GetOwner()->AddNewActor(bubble);
	}
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	TickShieldBlink(deltaTime);

	UpdateStateTick(deltaTime);

	if (currentState == PlayerState::DEAD)
		return;

	if (CanMove())
		HandleMovementInput(deltaTime);

	if (CanAct())
		HandleActionInput();

}

void Player::Draw()
{
	super::Draw();
}

void Player::OnEnterNormal()
{
	moveSpeed = MoveSpeed::NORMAL;
	SetSprite(L"P", Color::Green);
}

void Player::OnEnterBubbleTrap()
{
	moveSpeed = MoveSpeed::SLOW;
	bubbleTrapTimer.Reset();
	bubbleTrapTimer.SetTargetTime(bubbleTrapDuration);
	SetSprite(L"0", Color::Skyblue);
}

void Player::OnEnterDead()
{
	SetSprite(L"X", Color::Green);
}

void Player::OnDamaged()
{
	if (UseShieldItem())
		return;

	if (currentState == PlayerState::NORMAL)
	{
		if (lives > 0)
			ChangeState(PlayerState::TRAPPED_IN_BUBBLE);
		else
			ChangeState(PlayerState::DEAD);
	}
	else if (currentState == PlayerState::TRAPPED_IN_BUBBLE)
		ChangeState(PlayerState::DEAD);
}

void Player::OnKilled()
{
	if (UseShieldItem())
		return;
	lives = 0;
	ChangeState(PlayerState::DEAD);
}