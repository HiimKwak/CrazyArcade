#include "ExplosionTilePool.h"
#include "ExplosionTile.h"

ExplosionTilePool::ExplosionTilePool(size_t initialSize)
{
	freeList.reserve(initialSize);
	allSegments.reserve(initialSize);

	for (size_t i = 0; i < initialSize; ++i)
	{
		ExplosionTile* seg = new ExplosionTile(engine::Vector2::Zero);
		seg->SetPool(this);
		seg->Deactivate();
		freeList.push_back(seg);
		allSegments.push_back(seg);
	}
}

ExplosionTilePool::~ExplosionTilePool()
{
	for (ExplosionTile* seg : allSegments)
	{
		delete seg;
	}
	allSegments.clear();
	freeList.clear();
}

ExplosionTile* ExplosionTilePool::Acquire(const engine::Vector2& tilePos, float lifetime)
{
	ExplosionTile* segment = nullptr;

	if (freeList.size() > 0)
	{
		segment = freeList.back();
		freeList.pop_back();
		segment->Reset(tilePos, lifetime);
	}
	else
	{
		segment = new ExplosionTile(tilePos, lifetime);
		segment->SetPool(this);
		allSegments.push_back(segment);
	}

	segment->Activate();
	return segment;
}

void ExplosionTilePool::Release(ExplosionTile* segment)
{
	if (!segment) return;

	segment->Deactivate();
	freeList.push_back(segment);
}
