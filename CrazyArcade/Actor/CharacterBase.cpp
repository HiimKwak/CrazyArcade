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
		moveSpeed -= 0.01;
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

bool CharacterBase::UseShieldItem()
{
	if (isBlinking) // 쉴드 지속시간동안 피격 무시
		return true;

	if (!hasShield)
		return false;

	hasShield = false;

	isBlinking = true;
	shieldTimer.SetTargetTime(shieldBlinkDuration);
	shieldTimer.Reset();	

	return true;

}
void CharacterBase::TickShieldBlink(float deltaTime)
{
	if (!isBlinking)
		return;

	shieldTimer.Tick(deltaTime);

	float elapsedTime = shieldBlinkDuration - shieldTimer.GetRemainingTime();
	int blinkCount = static_cast<int>(elapsedTime / shieldBlinkInterval);

	if (blinkCount % 2 == 0)
		super::SetSpriteColor(Color::White);
	else
		super::SetSpriteColor(originalColor);

	if (shieldTimer.IsTimeout())
	{
		isBlinking = false;
		super::SetSpriteColor(originalColor);
	}
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