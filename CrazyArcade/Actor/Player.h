#pragma once

#include <vector>
#include "Util/Timer.h"
#include "Actor/Actor.h"

using namespace engine;

class Bubble;

namespace MoveSpeed {
	constexpr float FAST = 0.0125f;
	constexpr float NORMAL = 0.125f;
	constexpr float SLOW = 0.75f;
};

enum PlayerState {
	NORMAL,
	TRAPPED_IN_BUBBLE,
	DEAD
};

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public:
	Player(const Vector2& position);

	void OnDamaged();
	void OnBubbleExploded();
	void TrappedInBubble();

	inline bool IsDead() const { return currentState == DEAD; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	void HandleMovementInput(float deltaTime);
	void SetMoveTarget(Vector2& direction);

	void HandleActionInput();

	PlayerState currentState = NORMAL;

	Timer moveTimer;
	float moveSpeed = MoveSpeed::NORMAL;
	Vector2 moveStartPos;
	Vector2 moveTargetPos;
	bool isMoving = false;
	float moveProgress = 0.0f;

	Timer bubbleTrapTimer;
	float bubbleTrapDuration = 3.0f;
	int remainingBubbleEscapeCount = 2;

	static constexpr int maxBubbleRounds = 3;
	int bubbleAmmo = maxBubbleRounds;
};
