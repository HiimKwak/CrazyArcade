#include "Bubble.h"
#include "Level/Level.h"
#include "BubbleExplosion.h"
#include "Player.h"

Bubble::Bubble(const Vector2& newPosition, int explosionRange)
	: super(L"●", newPosition, Color::Blue),
	range(explosionRange)
{
	sortingOrder = 5;
	explodeTimer.SetTargetTime(countDown);
	explodeTimer.Reset();
}

void Bubble::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	Explode(deltaTime);
}

bool Bubble::IsExploded() const
{
	return exploded;
}

void Bubble::Explode(float deltaTime)
{
	explodeTimer.Tick(deltaTime);

	if (!explodeTimer.IsTimeout())
	{
		float remainingTime = explodeTimer.GetRemainingTime();
		int blink = 0;
		if (remainingTime <= 1.0f)
			blink = static_cast<int>(remainingTime * 10) % 2;

		else if (remainingTime <= 2.0f)
			blink = static_cast<int>(remainingTime * 5) % 2;

		else if (remainingTime <= 3.0f)
			blink = static_cast<int>(remainingTime * 3.33f) % 2;

		return this->sprite.SetColor(blink == 0 ? Color::Blue : Color::Skyblue);
	}

	explodeTimer.Reset();
	exploded = true;

	if (owner)
		owner->AddNewActor(new BubbleExplosion(GetPosition(), range));

	if (ownerCharacter)
		ownerCharacter->OnBubbleExploded();

	Destroy();
}
