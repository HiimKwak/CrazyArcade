#include "CharacterBase.h"

#include "Level/Level.h"
#include "Actor/Item.h"
#include "Interface/IGameRuleManager.h"

using namespace engine;

void CharacterBase::BeginPlay()
{
	super::BeginPlay();
	gameRuleManager = dynamic_cast<IGameRuleManager*>(GetOwner());
}

void CharacterBase::AddItem(ItemType type)
{
	inventory.push_back(type);

	switch (type)
	{
	case ItemType::BubbleUpgrade:
		bubbleRange++;
		break;

	case ItemType::Roller:
		moveSpeed = MoveSpeed::FAST;
		break;

	case ItemType::Shield:
		hasShield = true;
		break;
	}
}

void CharacterBase::OnBubbleExploded()
{
	if (bubbleAmmo < maxBubbleAmmo)
		bubbleAmmo++;
}

bool CharacterBase::ConsumeShieldIfAny()
{
	if (!hasShield)
		return false;

	hasShield = false;
	// todo: ½¯µå ±úÁö´Â ÀÌÆåÆ®
	return true;
}

void CharacterBase::TickMovementInterpolation(float deltaTime)
{
	if (!isMoving)
		return;

	moveTimer.Tick(deltaTime);

	moveProgress = 1.0f - (moveTimer.GetRemainingTime() / moveSpeed);
	moveProgress = Util::Clamp(moveProgress, 0.0f, 1.0f);

	Vector2 currentPos = Vector2::Lerp(moveStartPos, moveTargetPos, moveProgress);
	SetPosition(currentPos);

	if (moveTimer.IsTimeout())
	{
		SetPosition(moveTargetPos);
		isMoving = false;
	}
}