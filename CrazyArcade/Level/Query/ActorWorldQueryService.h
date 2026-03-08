#pragma once

#include "WorldQueryService.h"
#include "Actor/Actor.h"
#include "Actor/Effects/Bubble.h"
#include "Actor/Effects/ExplosionTile.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Environments/Box.h"

#include <limits>

using namespace engine;

class ActorWorldQueryService : public WorldQueryService
{
public:
	void BindActors(const std::vector<Actor*>* inActors)
	{
		actors = inActors;
	}

	CollisionTargets CollectCollisionTargets() const override
	{
		CollisionTargets out;
		if (!actors) return out;

		for (Actor* actor : *actors)
		{
			if (actor->IsTypeOf<ExplosionTile>())
			{
				ExplosionTile* seg = actor->As<ExplosionTile>();
				if (seg->IsActive())
					out.activeExplosions.push_back(seg);
			}

			if (actor->IsTypeOf<Player>())
				out.player = actor->As<Player>();

			if (actor->IsTypeOf<Enemy>())
				out.enemies.push_back(actor->As<Enemy>());

			if (actor->IsTypeOf<Box>())
				out.boxes.push_back(actor->As<Box>());
		}

		return out;
	}

	bool HasBubbleAt(const Vector2& position) const override
	{
		if (!actors) return false;
		for (Actor* actor : *actors)
		{
			if (actor->IsTypeOf<Bubble>() && actor->GetPosition() == position)
				return true;
		}
		return false;
	}

	bool HasPlayerAt(const Vector2& position) const override
	{
		if (!actors) return false;
		for (Actor* actor : *actors)
		{
			if (actor->IsTypeOf<Player>() && actor->GetPosition() == position)
				return true;
		}
		return false;
	}

	bool HasExplosionAt(const Vector2& position) const override
	{
		if (!actors) return false;
		for (Actor* actor : *actors)
		{
			if (actor->IsTypeOf<ExplosionTile>() && actor->GetPosition() == position)
				return true;
		}
		return false;
	}

	bool HasBoxAt(const Vector2& position) const override
	{
		if (!actors) return false;
		for (Actor* actor : *actors)
		{
			if (actor->IsTypeOf<Box>() && actor->GetPosition() == position)
				return true;
		}
		return false;
	}

	Vector2 GetPlayerPosition() const override
	{
		if (!actors) return Vector2::Zero;
		for (Actor* actor : *actors)
		{
			if (actor->IsTypeOf<Player>())
				return actor->As<Player>()->GetPosition();
		}
		return Vector2::Zero;
	}

	bool IsExplosionDangerAt(const Vector2& position) const override
	{
		if (HasExplosionAt(position))
			return true;

		if (!actors) return false;
		for (Actor* actor : *actors)
		{
			if (!actor->IsTypeOf<Bubble>()) continue;
			for (const Vector2& tile : actor->As<Bubble>()->GetPredictedDangerTiles())
			{
				if (tile == position) return true;
			}
		}
		return false;
	}

	float GetExplosionTimeAt(const Vector2& position) const override
	{
		if (HasExplosionAt(position))
			return 0.0f;

		float minTime = std::numeric_limits<float>::infinity();
		if (!actors) return minTime;

		for (Actor* actor : *actors)
		{
			if (!actor->IsTypeOf<Bubble>()) continue;
			Bubble* bubble = actor->As<Bubble>();
			for (const Vector2& tile : bubble->GetPredictedDangerTiles())
			{
				if (tile == position)
				{
					float t = bubble->GetRemainingTime();
					if (t < minTime) minTime = t;
					break;
				}
			}
		}
		return minTime;
	}

private:
	const std::vector<Actor*>* actors = nullptr;
};
