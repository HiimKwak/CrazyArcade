#pragma once
#include "Util/Timer.h"
#include "Actor/Actor.h"

using namespace engine;

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

	enum PlayerState {
		NORMAL,
		TRAPPED_IN_BUBBLE,
		DEAD
	};

	struct MoveSpeed {
		static constexpr float FAST = 0.0125f;
		static constexpr float NORMAL = 0.125f;
		static constexpr float SLOW = 0.75f;
	};

public:
	Player(const Vector2& position);

	void OnDamaged();

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
};
