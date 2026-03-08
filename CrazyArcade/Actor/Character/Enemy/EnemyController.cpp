#include "EnemyController.h"
#include "Enemy.h"
#include "../Component/State/StateComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/BubbleComponent.h"
#include "../Component/StatsComponent.h"

#include "AI/BTComposite.h"
#include "AI/BTDecorator.h"
#include "AI/BT/Conditions/IsMovingCondition.h"
#include "AI/BT/Conditions/HasBubbleAmmoCondition.h"
#include "AI/BT/Conditions/IsDangerNearbyCondition.h"
#include "AI/BT/Conditions/IsPlayerAdjacentCondition.h"
#include "AI/BT/Actions/FleeFromDangerAction.h"
#include "AI/BT/Actions/AStarMoveAction.h"
#include "AI/BT/Actions/PlaceBubbleAction.h"
#include "AI/BT/Actions/PlaceBubbleNearBoxAction.h"

using namespace engine;

EnemyController::EnemyController(Enemy* owner)
	: owner(owner)
{
	auto* movement = owner->GetComponent<MovementComponent>();
	auto* stats    = owner->GetComponent<StatsComponent>();
	auto* bubble   = owner->GetComponent<BubbleComponent>();

	// movement tree
	{
		auto innerBoxSeq = std::make_unique<BTSequence>();
		innerBoxSeq->AddChild(std::make_unique<HasBubbleAmmoCondition>(stats));
		innerBoxSeq->AddChild(std::make_unique<PlaceBubbleNearBoxAction>(owner, movement, bubble));

		auto innerSel = std::make_unique<BTSelector>();
		innerSel->AddChild(std::make_unique<AStarMoveAction>(owner, movement));
		innerSel->AddChild(std::move(innerBoxSeq));

		auto cooldown = std::make_unique<BTCooldown>(0.5f);
		cooldown->SetChild(std::move(innerSel));

		auto dangerSeq = std::make_unique<BTSequence>();
		dangerSeq->AddChild(std::make_unique<IsDangerNearbyCondition>(owner));
		dangerSeq->AddChild(std::make_unique<FleeFromDangerAction>(owner, movement));

		auto root = std::make_unique<BTSelector>();
		root->AddChild(std::move(dangerSeq));
		root->AddChild(std::make_unique<IsMovingCondition>(movement));
		root->AddChild(std::move(cooldown));

		movementTree = std::move(root);
	}

	// action tree
	{
		auto cooldown = std::make_unique<BTCooldown>(1.5f);
		cooldown->SetChild(std::make_unique<PlaceBubbleAction>(bubble));

		auto root = std::make_unique<BTSequence>();
		root->AddChild(std::make_unique<IsPlayerAdjacentCondition>(owner));
		root->AddChild(std::move(cooldown));

		actionTree = std::move(root);
	}
}

void EnemyController::Tick(float deltaTime)
{
	if (!owner) return;

	auto stateComp = owner->GetComponent<StateComponent>();
	if (!stateComp) return;

	if (stateComp->CanMove())
		movementTree->Execute(deltaTime);

	if (stateComp->CanAct())
		actionTree->Execute(deltaTime);
}
