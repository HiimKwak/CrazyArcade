#pragma once

#include <vector>
#include "Math/Vector2.h"

class ExplosionTile;

// todo: 추상화해서 ObjectPool 해보기?
class ExplosionTilePool
{
public:
	ExplosionTilePool(size_t initialSize = 64);
	~ExplosionTilePool();

	ExplosionTile* Acquire(const engine::Vector2& tilePos, float lifetime = 1.0f);
	void Release(ExplosionTile* segment);

	inline size_t GetFreeCount() const { return freeList.size(); }
	inline size_t GetTotalCount() const { return allSegments.size(); }

private:
	std::vector<ExplosionTile*> freeList;
	std::vector<ExplosionTile*> allSegments;
};
