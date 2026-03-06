#include "MovementComponent.h"
#include "../Character.h"
#include "Util/Util.h"

void MovementComponent::Tick(float deltaTime)
{
	if (!moving)
		return;

	moveTimer += deltaTime;
	moveProgress = moveTimer / moveSpeed;
	moveProgress = Util::Clamp(moveProgress, 0.0f, 1.0f);

	Vector2 interpolatedPos = Vector2::Lerp(moveStart, moveTarget, moveProgress);
	if (owner)
	{
		owner->SetPosition(interpolatedPos);
	}

	if (moveProgress >= 1.0f)
	{
		moving = false;
		if (owner)
		{
			owner->SetPosition(moveTarget);
		}
	}
}

void MovementComponent::StartMove(const Vector2& targetPosition)
{
	if (moving)
		return;

	if (owner)
	{
		moveStart = owner->GetPosition();
	}
	moveTarget = targetPosition;
	moveProgress = 0.0f;
	moveTimer = 0.0f;
	moving = true;
}

bool MovementComponent::RequestMove(const Vector2& direction)
{
	if (!owner || moving)
		return false;

	auto delegate = GetCharacter()->GetDelegate();
	if (!delegate)
		return false;

	Vector2 targetPos = owner->GetPosition() + direction;

	if (delegate->OnRequestMove(GetCharacter(), targetPos))
	{
		StartMove(targetPos);
		return true;
	}
	return false;
}
