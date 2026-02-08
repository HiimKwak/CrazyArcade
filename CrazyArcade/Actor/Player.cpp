#include <iostream>
#include <Windows.h>

//#include "Engine/Engine.h"
#include "Player.h"
#include "Core/Input.h"
#include "Game/Game.h"
#include "Level/GameLevel.h"
#include "Actor/Bubble.h"
#include "Util/Timer.h"

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

static IGameRuleManager* GameRuleManager = nullptr;

void Player::TryMove(Vector2& vector)
{
	Vector2 newPosition(GetPosition() + vector);
	if (GameRuleManager->CanMove(GetPosition(), newPosition))
		SetPosition(newPosition);
}

void Player::HandleMovementInput()
{
	if (!GetOwner())
		return;
	
	GameRuleManager = dynamic_cast<IGameRuleManager*>(GetOwner());

	if (!GameRuleManager)
		return;

	if (Input::Get().GetKeyDown(VK_RIGHT))
		TryMove(Vector2::Right);
	if (Input::Get().GetKeyDown(VK_LEFT))
		TryMove(Vector2::Left);
	if (Input::Get().GetKeyDown(VK_DOWN))
		TryMove(Vector2::Up);
	if (Input::Get().GetKeyDown(VK_UP))
		TryMove(Vector2::Down);
}

void Player::HandleActionInput()
{
	if (!GetOwner())
		return;

	if (Input::Get().GetKeyDown((VK_SPACE)))
	{
		Bubble* bubble = new Bubble(GetPosition());

		GameLevel* gameLevel = dynamic_cast<GameLevel*>(GetOwner());
		ExplosionTilePool* gameLevelPool = gameLevel->GetExplosionTilePool();

		if (gameLevel && gameLevelPool)
			bubble->SetExplosionTilePool(gameLevelPool);

		GetOwner()->AddNewActor(bubble);
	}
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (currentState != NORMAL)
		return;

	HandleMovementInput();
	HandleActionInput();
}



void Player::Draw()
{
	super::Draw();
}

void Player::TrappedInBubble()
{
	currentState = TRAPPED_IN_BUBBLE;
	moveSpeed = 1.0f;
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