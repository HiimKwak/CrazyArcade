#include "Level/Level.h"
#include "BubbleExplosion.h"
#include "ExplosionTile.h"
#include "ExplosionTilePool.h"

BubbleExplosion::BubbleExplosion(const Vector2& origin, int explosionRange, ExplosionTilePool* pool)
	: super("", origin, Color::White),
	range(explosionRange),
	segmentPool(pool),
	propagated(false)
{
	sortingOrder = 0;
}

void BubbleExplosion::BeginPlay()
{
	super::BeginPlay();
	Propagate();
}

void BubbleExplosion::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (propagated)
		Destroy(); // BubbleExplosion은 폭발 범위만 나타내고 제거시킴
}

void BubbleExplosion::Propagate()
{
	if (propagated) return;

	Vector2 origin = GetPosition();

	if (segmentPool)
	{
		ExplosionTile* center = segmentPool->Acquire(origin, 1.0f); // todo: explosion lifetime magic number
		segments.push_back(center);
	}

	PropagateInDirection(Vector2::Up, range);
	PropagateInDirection(Vector2::Right, range);
	PropagateInDirection(Vector2::Down, range);
	PropagateInDirection(Vector2::Left, range);

	propagated = true;
}

void BubbleExplosion::PropagateInDirection(const Vector2& direction, int range)
{
	if (!segmentPool) return;

	Vector2 origin = GetPosition();

	for (int i = 1; i <= range; ++i)
	{
		Vector2 tilePos = origin + (i * direction);

		ExplosionTile* seg = segmentPool->Acquire(tilePos, 1.0f);
		segments.push_back(seg);

		if (owner)
			owner->AddNewActor(seg);
	}
}
