#include "Bubble.h"
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
		// todo: 폭발시간이 가까워질수록 깜빡이는 효과
		return;
	explodeTimer.Reset();
	exploded = true;

	if (segmentPool)
		BubbleExplosion* explosion = new BubbleExplosion(GetPosition(), range, segmentPool);

	Destroy();
}
