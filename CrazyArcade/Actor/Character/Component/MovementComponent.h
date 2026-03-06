#pragma once

#include "ICharacterComponent.h"
#include "Math/Vector2.h"

using namespace engine;

class MovementComponent : public ICharacterComponent
{
public:
	virtual void Tick(float deltaTime) override;

	void StartMove(const Vector2& targetPosition);
	bool RequestMove(const Vector2& direction);
	bool IsMoving() const { return moving; }
	float GetMoveSpeed() const { return moveSpeed; }
	void SetMoveSpeed(float speed) { moveSpeed = speed; }

	Vector2 GetCurrentPosition() const { return currentPosition; }
	void SetCurrentPosition(const Vector2& pos) { currentPosition = pos; }

protected:
	Vector2 moveStart;
	Vector2 moveTarget;
	Vector2 currentPosition;
	float moveSpeed = 0.2f;
	float moveProgress = 0.0f;
	float moveTimer = 0.0f;
	bool moving = false;
};
