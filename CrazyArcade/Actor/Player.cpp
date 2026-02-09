#include <iostream>
#include <Windows.h>

#include "Player.h"
#include "Core/Input.h"
#include "Game/Game.h"
#include "Level/GameLevel.h"
#include "Actor/Bubble.h"
#include "Util/Timer.h"
#include "Math/Vector2.h"

#include "Interface/IGameRuleManager.h"

using namespace engine;

Player::Player(const Vector2& position)
	: super("P", position, Color::Red)
{
	sortingOrder = 10;
}

void Player::BeginPlay()
{
	super::BeginPlay();
}

static IGameRuleManager* gameRuleManager = nullptr;

void Player::SetMoveTarget(Vector2& direction)
{
	if (isMoving)
		return;

	Vector2 targetPosition(GetPosition() + direction);
	if (gameRuleManager->CanMove(GetPosition(), targetPosition))
	{
		moveStartPos = GetPosition();
		moveTargetPos = targetPosition;
		isMoving = true;
		moveProgress = 0.0f;
		moveTimer.Reset();
		moveTimer.SetTargetTime(moveSpeed);
	}
}

void Player::HandleMovementInput(float deltaTime)
{
	if (isMoving)
	{
		moveTimer.Tick(deltaTime);

		moveProgress = 1.0f - (moveTimer.GetRemainingTime() / moveSpeed);

		float eased = 1.0f - (1.0f - moveProgress) * (1.0f - moveProgress);

		Vector2 currentPos = Vector2::Lerp(moveStartPos, moveTargetPos, eased);
		SetPosition(currentPos);

		if (moveTimer.IsTimeout())
		{
			SetPosition(moveTargetPos);  // 정확한 위치로 스냅
			isMoving = false;
		}
	}
	else
	{
		if (!GetOwner())
			return;

		gameRuleManager = dynamic_cast<IGameRuleManager*>(GetOwner());

		if (!gameRuleManager)
			return;

		if (Input::Get().GetKey(VK_RIGHT))
			SetMoveTarget(Vector2::Right);
		else if (Input::Get().GetKey(VK_LEFT))
			SetMoveTarget(Vector2::Left);
		else if (Input::Get().GetKey(VK_DOWN))
			SetMoveTarget(Vector2::Up);
		else if (Input::Get().GetKey(VK_UP))
			SetMoveTarget(Vector2::Down);
	}
}

void Player::HandleActionInput()
{
	if (!GetOwner())
		return;

	gameRuleManager = dynamic_cast<IGameRuleManager*>(GetOwner());

	if (Input::Get().GetKey((VK_SPACE)) && 0 < bubbleAmmo)
	{
		if (gameRuleManager && gameRuleManager->HasBubbleAt(GetPosition()))
			return;

		bubbleAmmo--;
		Bubble* bubble = new Bubble(GetPosition());
		bubble->SetOwnerPlayer(this);

		GetOwner()->AddNewActor(bubble);
	}
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (currentState != NORMAL)
		return;

	HandleMovementInput(deltaTime);
	HandleActionInput();
}

void Player::Draw()
{
	super::Draw();
}

void Player::TrappedInBubble()
{
	currentState = TRAPPED_IN_BUBBLE;
	moveSpeed = MoveSpeed::SLOW;
	bubbleTrapTimer.SetTargetTime(bubbleTrapDuration);
	remainingBubbleEscapeCount--;
	SetSprite("0", Color::Blue);
}

void Player::OnDamaged()
{
	if (currentState == NORMAL)
	{
		if (remainingBubbleEscapeCount > 0)
		{
			TrappedInBubble();
		}
		else
		{
			currentState = DEAD;
		}
	}
	else if (currentState == TRAPPED_IN_BUBBLE)
	{
		currentState = DEAD;
	}
}

void Player::OnBubbleExploded()
{
	if (bubbleAmmo < maxBubbleRounds)
		bubbleAmmo++;
}
