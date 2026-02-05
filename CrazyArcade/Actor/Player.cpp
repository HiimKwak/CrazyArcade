#include <iostream>
#include <Windows.h>

#include "Player.h"
#include "Core/Input.h"
#include "Engine/Engine.h"
#include "Actor/Bubble.h"
#include "Level/Level.h"
#include "Game/Game.h"

#include "Interface/IGameRuleManager.h"

using namespace engine;

Player::Player(const Vector2& position)
	: super("P", position, Color::Red)
{
	sortingOrder = 10;
}

void Player::BeginPlay()
{
	Actor::BeginPlay();
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		Game::Get().ToggleMenu();
		return;
	}

	if (Input::Get().GetKeyDown('Q'))
	{
		Engine::Get().Quit();
	}

	if (Input::Get().GetKeyDown((VK_SPACE)))
	{
		if (owner)
		{
			owner->AddNewActor(new Bubble(GetPosition()));
		}
	}

	static IGameRuleManager* canPlayerMoveInterface = nullptr;
	if (GetOwner() && !canPlayerMoveInterface)
	{
		canPlayerMoveInterface = dynamic_cast<IGameRuleManager*>(GetOwner());
	}

	if (Input::Get().GetKeyDown(VK_RIGHT) && GetPosition().x < Engine::Get().GetWidth())
	{
		Vector2 newPosition(GetPosition().x + 1, GetPosition().y);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition))
			SetPosition(newPosition);

	}
	if (Input::Get().GetKeyDown(VK_LEFT) && GetPosition().x > 0)
	{
		Vector2 newPosition(GetPosition().x - 1, GetPosition().y);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition))
			SetPosition(newPosition);

	}
	if (Input::Get().GetKeyDown(VK_DOWN) && GetPosition().y < Engine::Get().GetHeight())
	{
		Vector2 newPosition(GetPosition().x, GetPosition().y + 1);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition))
			SetPosition(newPosition);
	}
	if (Input::Get().GetKeyDown(VK_UP) && GetPosition().y > 0)
	{
		Vector2 newPosition(GetPosition().x, GetPosition().y - 1);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition))
			SetPosition(newPosition);
	}
}

void Player::Draw()
{
	Actor::Draw();
}

void Player::OnDamaged()
{
	// todo: bubble capsule

	Destroy();

	// todo: effect
}