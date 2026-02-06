#include "Bubble.h"
#include "Level/Level.h"
#include "BubbleExplosion.h"
#include "ExplosionTilePool.h"

Bubble::Bubble(const Vector2& newPosition, int explosionRange)
	: super("@", newPosition, Color::Blue),
	range(explosionRange),
	segmentPool(nullptr)
{
	sortingOrder = 5;
	explodeTimer.SetTargetTime(countDown);
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

void Bubble::SetExplosionTilePool(ExplosionTilePool* pool)
{
	segmentPool = pool;
}

void Bubble::Explode(float deltaTime)
{
	explodeTimer.Tick(deltaTime);

	if (!explodeTimer.IsTimeout())
	{
		float remainingTime = explodeTimer.GetRemainingTime();
		if (remainingTime <= 1.0f)
		{
			int blink = static_cast<int>(remainingTime * 10) % 2;
			this->color = blink == 0 ? Color::Blue : Color::Skyblue;
		}
		else if (remainingTime <= 2.0f)
		{
			int blink = static_cast<int>(remainingTime * 5) % 2;
			this->color = blink == 0 ? Color::Blue : Color::Skyblue;
		}
		else if (remainingTime <= 3.0f)
		{
			int blink = static_cast<int>(remainingTime * 3.33f) % 2;
			this->color = blink == 0 ? Color::Blue : Color::Skyblue;
		}

		return;
	}

	explodeTimer.Reset();
	exploded = true;

	if (segmentPool)
	{
		BubbleExplosion* explosion = new BubbleExplosion(GetPosition(), range, segmentPool);
		if (owner)
			owner->AddNewActor(explosion);
	}

	Destroy();
}
