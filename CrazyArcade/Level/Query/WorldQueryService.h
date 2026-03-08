#pragma once

#include <vector>
#include "Math/Vector2.h"

using namespace engine;

class Player;
class Enemy;
class Box;
class ExplosionTile;

struct CollisionTargets
{
	std::vector<ExplosionTile*> activeExplosions;
	Player* player = nullptr;
	std::vector<Enemy*> enemies;
	std::vector<Box*> boxes;
};

class WorldQueryService
{
public:
	virtual ~WorldQueryService() = default;

	virtual CollisionTargets CollectCollisionTargets() const = 0;

	virtual bool HasBubbleAt(const Vector2& position) const = 0;
	virtual bool HasPlayerAt(const Vector2& position) const = 0;
	virtual bool HasExplosionAt(const Vector2& position) const = 0;
	virtual bool HasBoxAt(const Vector2& position) const = 0;

	virtual Vector2 GetPlayerPosition() const = 0;

	virtual bool IsExplosionDangerAt(const Vector2& position) const = 0;
	virtual float GetExplosionTimeAt(const Vector2& position) const = 0;
};
