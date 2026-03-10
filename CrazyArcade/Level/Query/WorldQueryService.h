#pragma once

#include <vector>
#include <limits>

#include "Math/Vector2.h"
#include "Interface/ActorType.h"
#include "Actor/Actor.h"
#include "Actor/Effects/WaterBalloon.h"
#include "Actor/Effects/ExplosionTile.h"
#include "Actor/Character/Character.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Environments/Box.h"
#include "Actor/Character/Component/State/StateComponent.h"
#include "Interface/IWorldQueryService.h"
#include "Actor/Character/Component/Item/ItemComponent.h"
#include "Actor/Environments/Wall.h"
#include "Engine/Engine.h"

using namespace engine;

struct CollisionTargets
{
	std::vector<ExplosionTile*> activeExplosions;
	Player* player = nullptr;
	std::vector<Enemy*> enemies;
	std::vector<Box*> boxes;
};

class WorldQueryService : public CharacterQueryDelegate, public IWorldQueryService
{
public:
	void BindActors(const std::vector<Actor*>* inActors)
	{
		actors = inActors;
	}

	void BindMapBounds(const Vector2* inMapOrigin, const int* inMapWidth, const int* inMapHeight)
	{
		mapOrigin = inMapOrigin;
		mapWidth = inMapWidth;
		mapHeight = inMapHeight;
	}

	CollisionTargets CollectCollisionTargets() const
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

	bool HasActorAt(const Vector2& position, ActorType type) const override
	{
		if (!actors) return false;

		for (Actor* actor : *actors)
		{
			if (actor->GetPosition() != position)
				continue;

			if (IsMatchForType(actor, type))
				return true;
		}
		return false;
	}

	bool HasBoxAt(const Vector2& position) const override
	{
		return HasActorAt(position, ActorType::Box);
	}

	bool HasExplosionAt(const Vector2& position) const override
	{
		return HasActorAt(position, ActorType::Explosion);
	}

	bool CanExplosionPenetrate(const Vector2& position) const override
	{
		if (!actors)
			return true;

		for (Actor* actor : *actors)
		{
			if (actor->GetPosition() == position && (actor->IsTypeOf<Wall>() || actor->IsTypeOf<WaterBalloon>()))
				return false;
		}
		return true;
	}

	bool IsInsideGameMap(const Vector2& position) const override
	{
		if (!mapOrigin || !mapWidth || !mapHeight)
			return true;

		if (*mapWidth <= 0 || *mapHeight <= 0)
			return false;

		const int ts = engine::Engine::Get().GetTileSize();
		const int left = mapOrigin->x;
		const int top = mapOrigin->y;
		const int right = mapOrigin->x + (*mapWidth * ts - 1);
		const int bottom = mapOrigin->y + (*mapHeight * ts - 1);

		return (position.x >= left && position.x <= right &&
			position.y >= top && position.y <= bottom);
	}

	Vector2 GetActorPosition(ActorType type) const
	{
		if (!actors) return Vector2::Zero;

		for (Actor* actor : *actors)
		{
			if (type == ActorType::Player && actor->IsTypeOf<Player>())
				return actor->As<Player>()->GetPosition();
			if (type == ActorType::Enemy && actor->IsTypeOf<Enemy>())
				return actor->As<Enemy>()->GetPosition();
		}
		return Vector2::Zero;
	}

	Vector2 GetPlayerPosition() const
	{
		return GetActorPosition(ActorType::Player);
	}

	bool IsExplosionDangerAt(const Vector2& position) const
	{
		if (HasActorAt(position, ActorType::Explosion))
			return true;

		if (!actors) return false;
		for (Actor* actor : *actors)
		{
			if (!actor->IsTypeOf<WaterBalloon>()) continue;
			for (const Vector2& tile : actor->As<WaterBalloon>()->GetPredictedDangerTiles())
			{
				if (tile == position) return true;
			}
		}
		return false;
	}

	float GetExplosionTimeAt(const Vector2& position) const
	{
		if (HasActorAt(position, ActorType::Explosion))
			return 0.0f;

		float minTime = std::numeric_limits<float>::infinity();
		if (!actors) return minTime;

		for (Actor* actor : *actors)
		{
			if (!actor->IsTypeOf<WaterBalloon>()) continue;
			WaterBalloon* waterBalloon = actor->As<WaterBalloon>();
			for (const Vector2& tile : waterBalloon->GetPredictedDangerTiles())
			{
				if (tile == position)
				{
					float t = waterBalloon->GetRemainingTime();
					if (t < minTime) minTime = t;
					break;
				}
			}
		}
		return minTime;
	}

	bool IsActorBubbled(ActorType type) const
	{
		if (!actors) return false;

		for (Actor* actor : *actors)
		{
			if (!actor->IsTypeOf<Character>())
				continue;

			if (type == ActorType::Player && !actor->IsTypeOf<Player>())
				continue;
			if (type == ActorType::Enemy && !actor->IsTypeOf<Enemy>())
				continue;

			auto* stateComp = actor->As<Character>()->GetComponent<StateComponent>();
			if (!stateComp)
				continue;

			if (stateComp->GetCurrentState() == StateType::Bubbled)
				return true;

			if (type == ActorType::Player || type == ActorType::Enemy)
				return false;
		}

		return false;
	}

	Vector2 OnQueryActorPosition(ActorType type) const override
	{
		return GetActorPosition(type);
	}

	bool OnQueryHasWaterBalloonAt(const Vector2& position) const override
	{
		return HasActorAt(position, ActorType::WaterBalloon);
	}

	bool OnQueryHasBoxAt(const Vector2& position) const override
	{
		return HasActorAt(position, ActorType::Box);
	}

	bool OnQueryIsExplosionDangerAt(const Vector2& position) const override
	{
		return IsExplosionDangerAt(position);
	}

	float OnQueryExplosionTimeAt(const Vector2& position) const override
	{
		return GetExplosionTimeAt(position);
	}

	bool OnQueryIsActorBubbled(ActorType type) const override
	{
		return IsActorBubbled(type);
	}

	void SendItemToCharacter(const Vector2& itemPos, ItemType itemType) override
	{
		if (!actors)
			return;

		for (Actor* actor : *actors)
		{
			if (actor->GetPosition() != itemPos)
				continue;

			if (!actor->IsTypeOf<Character>())
				continue;

			Character* character = actor->As<Character>();
			auto stateComp = character->GetComponent<StateComponent>();
			if (stateComp && !stateComp->CanAct())
				continue;

			auto itemComp = character->GetComponent<ItemComponent>();
			if (itemComp)
			{
				itemComp->OnItemAcquired(itemType);
				break;
			}
		}
	}

private:
	bool IsMatchForType(Actor* actor, ActorType type) const
	{
		switch (type)
		{
		case ActorType::Player:
			return actor->IsTypeOf<Player>();
		case ActorType::Enemy:
			return actor->IsTypeOf<Enemy>();
		case ActorType::Character:
			return actor->IsTypeOf<Player>() || actor->IsTypeOf<Enemy>();
		case ActorType::WaterBalloon:
			return actor->IsTypeOf<WaterBalloon>();
		case ActorType::Box:
			return actor->IsTypeOf<Box>();
		case ActorType::Explosion:
			return actor->IsTypeOf<ExplosionTile>();
		default:
			return false;
		}
	}

private:
	const std::vector<Actor*>* actors = nullptr;
	const Vector2* mapOrigin = nullptr;
	const int* mapWidth = nullptr;
	const int* mapHeight = nullptr;
};
