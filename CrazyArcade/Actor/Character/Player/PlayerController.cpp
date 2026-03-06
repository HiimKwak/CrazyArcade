#include "PlayerController.h"
#include "Player.h"
#include "../Component/State/StateComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/BubbleComponent.h"
#include "../Component/ItemComponent.h"
#include "Core/Input.h"

using namespace engine;

PlayerController::PlayerController(Player* owner)
	: owner(owner)
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Tick(float deltaTime)
{
	if (!owner || !isInputActive)
		return;

	auto stateComp = owner->GetComponent<StateComponent>();
	if (!stateComp)
		return;

	if (stateComp->CanMove())
		HandleMovementInput(deltaTime);

	if (stateComp->CanAct())
	{
		HandleActionInput();
		HandleItemInput();
	}
}

void PlayerController::HandleMovementInput(float deltaTime)
{
	auto movementComp = owner->GetComponent<MovementComponent>();
	if (!movementComp || movementComp->IsMoving())
		return;

	if (Input::Get().GetKey(VK_RIGHT))
		movementComp->RequestMove(Vector2::Right);
	if (Input::Get().GetKey(VK_LEFT))
		movementComp->RequestMove(Vector2::Left);
	if (Input::Get().GetKey(VK_DOWN))
		movementComp->RequestMove(Vector2::Up);
	if (Input::Get().GetKey(VK_UP))
		movementComp->RequestMove(Vector2::Down);
}

void PlayerController::HandleActionInput()
{
	if (!Input::Get().GetKey(VK_SPACE))
		return;

	auto bubbleComp = owner->GetComponent<BubbleComponent>();
	if (bubbleComp)
		bubbleComp->RequestGenerateBubble();
}

void PlayerController::HandleItemInput()
{
	auto itemComp = owner->GetComponent<ItemComponent>();
	if (!itemComp)
		return;

	for (const auto& binding : itemKeyBindings)
	{
		if (Input::Get().GetKeyDown(binding.first))
		{
			itemComp->RequestUseItem(binding.second);
		}
	}
}

void PlayerController::BindItemToKey(int key, ItemType type)
{
	itemKeyBindings[key] = type;
}
