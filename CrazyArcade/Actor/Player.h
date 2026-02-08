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

public:
	Player(const Vector2& position);

	void OnDamaged();

	void TrappedInBubble();

	void HandleMovementInput();
	void HandleActionInput();

	inline bool IsDead() const { return currentState == DEAD; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	void TryMove(Vector2& vector);

	PlayerState currentState = NORMAL;
	float moveSpeed = 3.0f;

	Timer bubbleTrapTimer;
	float bubbleTrapDuration = 3.0f;
	int remainingBubbleEscapeCount = 2;
};
