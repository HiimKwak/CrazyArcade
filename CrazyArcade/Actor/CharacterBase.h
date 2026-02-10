#pragma once

#include <vector>

#include "Actor/Actor.h"
#include "Item.h"
#include "MoveSpeed.h"
#include "Math/Vector2.h"
#include "Util/Timer.h"

using namespace engine;

class IGameRuleManager;

class CharacterBase : public Actor
{
	RTTI_DECLARATIONS(CharacterBase, Actor)

public:
	CharacterBase(const wchar_t* image, const Vector2& position, Color color)
		: Actor(image, position, color), originalColor(color)
	{
	}

	virtual ~CharacterBase() = default;

	virtual void BeginPlay() override;

	virtual void AddItem(ItemType type);
	inline const std::vector<ItemType>& GetItems() const { return inventory; }

	virtual void OnBubbleExploded();

	inline int GetLives() const { return lives; }
	inline int GetBubbleAmmo() const { return bubbleAmmo; }
	inline int GetMaxBubbleAmmo() const { return maxBubbleAmmo; }
	inline int GetBubbleRange() const { return bubbleRange; }
	inline bool HasShield() const { return hasShield; }

protected:
	bool UseShieldItem();
	void TickMovementInterpolation(float deltaTime);
	void TickShieldBlink(float deltaTime);

protected:
	std::vector<ItemType> inventory;

	bool isMoving = false;
	Timer moveTimer;
	float moveSpeed = MoveSpeed::NORMAL;
	float moveProgress = 0.0f;
	Vector2 moveStartPos;
	Vector2 moveTargetPos;

	Timer bubbleTrapTimer;
	float bubbleTrapDuration = 3.0f;

	int lives = 1;

	int maxBubbleAmmo = 1;
	int bubbleAmmo = 1;
	int bubbleRange = 1;

	bool hasShield = false;
	Timer shieldTimer;
	float shieldBlinkDuration = 2.0f;
	float shieldBlinkInterval = 0.2f;
	bool isBlinking = false;
	Color originalColor;

	IGameRuleManager* gameRuleManager = nullptr;
};