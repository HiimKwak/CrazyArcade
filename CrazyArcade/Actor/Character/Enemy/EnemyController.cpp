#include "EnemyController.h"
#include "Enemy.h"
#include "../Component/State/StateComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/BubbleComponent.h"
#include "AI/EnemyBrain.h"

EnemyController::EnemyController(Enemy* owner)
	: owner(owner), brain(nullptr)
{
	brain = new EnemyBrain();
}

EnemyController::~EnemyController()
{
	delete brain;
}

void EnemyController::Tick(float deltaTime)
{
	if (!owner)
		return;

	auto stateComp = owner->GetComponent<StateComponent>();
	if (!stateComp)
		return;

	if (stateComp->CanMove())
		HandleMovement(deltaTime);

	if (stateComp->CanAct())
		HandleAction(deltaTime);
}

void EnemyController::HandleMovement(float deltaTime)
{
	auto movementComp = owner->GetComponent<MovementComponent>();
	if (!movementComp || movementComp->IsMoving())
		return;

	thinkTimer += deltaTime;
	if (thinkTimer < 0.5f)
		return;

	thinkTimer = 0.0f;

	Vector2 playerPos = owner->QueryPlayerPosition();
	desiredDirection = brain ? brain->ChooseDirection(owner->GetPosition(), playerPos, owner) : Vector2::Zero;

	if (desiredDirection != Vector2::Zero)
		movementComp->RequestMove(desiredDirection);
}

void EnemyController::HandleAction(float deltaTime)
{
	shootTimer += deltaTime;
	if (shootTimer < 3.0f)
		return;

	shootTimer = 0.0f;
	auto bubbleComp = owner->GetComponent<BubbleComponent>();
	if (bubbleComp)
		bubbleComp->RequestGenerateBubble();
}
