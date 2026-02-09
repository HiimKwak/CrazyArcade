#pragma once

#include <vector>
#include "Util/Timer.h"
#include "Actor/Actor.h"
#include "Actor/Item.h"

using namespace engine;

class Bubble;
class IGameRuleManager;

namespace MoveSpeed {
	constexpr float FAST = 0.0625f;
	constexpr float NORMAL = 0.125f;
	constexpr float SLOW = 0.5f;
};

enum class PlayerState {
	NORMAL,
	TRAPPED_IN_BUBBLE,
	DEAD
};

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public:
	Player(const Vector2& position);

	bool CanMove() const
	{
		return currentState == PlayerState::NORMAL || currentState == PlayerState::TRAPPED_IN_BUBBLE;
	}
	bool CanAct() const
	{
		return currentState == PlayerState::NORMAL;
	}

	inline bool IsDead() const { return currentState == PlayerState::DEAD; }
	inline PlayerState GetState() const { return currentState; }

	void AddItem(ItemType type);
	inline const std::vector<ItemType>& GetItems() const { return inventory; }

	void OnDamaged();
	void OnBubbleExploded();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	PlayerState currentState = PlayerState::NORMAL;
	void ChangeState(PlayerState newState);

	std::vector<ItemType> inventory;

private:
	void OnEnterBubbleTrap();
	void OnEnterDead();
	void OnEnterNormal();

	void TryMove(Vector2& direction);
	void UpdateStateTick(float deltaTime);
	void UpdateDeadTick(float deltaTime);
	void UpdateBubbleTrapTick(float deltaTime);
	void UpdateNormalTick(float deltaTime);
	void HandleMovementInput(float deltaTime);
	void HandleActionInput();

	bool isMoving = false;
	Timer moveTimer;
	float moveSpeed = MoveSpeed::NORMAL;
	float moveProgress = 0.0f;
	Vector2 moveStartPos;
	Vector2 moveTargetPos;

	Timer bubbleTrapTimer;
	float bubbleTrapDuration = 3.0f;
	int lives = 2;

	static constexpr int maxBubbleAmmo = 3;
	int bubbleAmmo = maxBubbleAmmo;
	int bubbleRange = 1;

	bool hasShield = false;

	IGameRuleManager* gameRuleManager = nullptr;
};
