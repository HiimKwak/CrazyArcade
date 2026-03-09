#include "EnemyController.h"
#include "Enemy.h"
#include "../Component/State/StateComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/BubbleComponent.h"
#include "../Component/StatsComponent.h"

#include "AI/BTComposite.h"

#include "./AI/BT/Conditions/IsMovingCondition.h"
#include "./AI/BT/Conditions/HasBubbleAmmoCondition.h"
#include "./AI/BT/Conditions/IsDangerNearbyCondition.h"
#include "./AI/BT/Conditions/IsPlayerAdjacentCondition.h"
#include "./AI/BT/Conditions/IsPlayerInAreaCondition.h"
#include "./AI/BT/Conditions/IsPlayerBubbleTrappedCondition.h"
#include "./AI/BT/Actions/FleeFromDangerAction.h"
#include "./AI/BT/Actions/ExploitDangerAction.h"
#include "./AI/BT/Actions/AStarMoveAction.h"
#include "./AI/BT/Actions/PlaceBubbleAction.h"
#include "./AI/BT/Actions/PlaceBubbleNearBoxAction.h"

using namespace engine;
using namespace enemy_ai;

EnemyController::EnemyController(Enemy* owner)
	: owner(owner)
{
	auto* movement = owner->GetComponent<MovementComponent>();
	auto* stats = owner->GetComponent<StatsComponent>();
	auto* bubble = owner->GetComponent<BubbleComponent>();

	constexpr int OFFENSIVE_AREA_TILES = 1;
	constexpr int DEFENSIVE_AREA_TILES = 3;
	constexpr int TRACING_AREA_MIN_TILES = DEFENSIVE_AREA_TILES + 1;

	// movement tree
	{
		auto innerBoxSeq = std::make_unique<BTSequence>();
		innerBoxSeq->AddChild(std::make_unique<HasBubbleAmmoCondition>(stats));
		innerBoxSeq->AddChild(std::make_unique<PlaceBubbleNearBoxAction>(owner, movement, bubble));

		auto offensiveApproachSeq = std::make_unique<BTSequence>();
		offensiveApproachSeq->AddChild(std::make_unique<IsPlayerBubbleTrappedCondition>(owner, true));
		offensiveApproachSeq->AddChild(std::make_unique<IsPlayerInAreaCondition>(owner, OFFENSIVE_AREA_TILES));
		offensiveApproachSeq->AddChild(std::make_unique<AStarMoveAction>(owner, movement));

		auto tracingApproachSeq = std::make_unique<BTSequence>();
		tracingApproachSeq->AddChild(std::make_unique<IsPlayerBubbleTrappedCondition>(owner, false));
		tracingApproachSeq->AddChild(std::make_unique<IsPlayerInAreaCondition>(owner, TRACING_AREA_MIN_TILES));
		tracingApproachSeq->AddChild(std::make_unique<AStarMoveAction>(owner, movement));

		auto innerSel = std::make_unique<BTSelector>();
		innerSel->AddChild(std::move(offensiveApproachSeq));
		innerSel->AddChild(std::move(tracingApproachSeq));
		innerSel->AddChild(std::move(innerBoxSeq));

		auto dangerSel = std::make_unique<BTSelector>();
		dangerSel->AddChild(std::make_unique<ExploitDangerAction>(owner, movement, bubble));
		dangerSel->AddChild(std::make_unique<FleeFromDangerAction>(owner, movement));

		auto dangerSeq = std::make_unique<BTSequence>();
		dangerSeq->AddChild(std::make_unique<IsDangerNearbyCondition>(owner));
		dangerSeq->AddChild(std::move(dangerSel));

		auto root = std::make_unique<BTSelector>();
		root->AddChild(std::move(dangerSeq));
		root->AddChild(std::make_unique<IsMovingCondition>(movement));
		root->AddChild(std::move(innerSel));

		movementTree = std::move(root);
	}

	// action tree
	{
		auto defensiveCombatSeq = std::make_unique<BTSequence>();
		defensiveCombatSeq->AddChild(std::make_unique<IsPlayerBubbleTrappedCondition>(owner, false));
		defensiveCombatSeq->AddChild(std::make_unique<IsPlayerInAreaCondition>(owner, OFFENSIVE_AREA_TILES + 1, DEFENSIVE_AREA_TILES));
		defensiveCombatSeq->AddChild(std::make_unique<HasBubbleAmmoCondition>(stats));
		defensiveCombatSeq->AddChild(std::make_unique<PlaceBubbleAction>(bubble));

		auto root = std::make_unique<BTSelector>();
		root->AddChild(std::move(defensiveCombatSeq));

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
