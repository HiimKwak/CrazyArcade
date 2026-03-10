#pragma once

#include "Math/Vector2.h"
#include "Interface/ActorType.h"
#include "Actor/Character/Component/Item/ItemType.h"

using namespace engine;

class IWorldQueryService
{
public:
	virtual ~IWorldQueryService() = default;

	virtual bool HasActorAt(const Vector2& position, ActorType type) const = 0;
	virtual bool HasBoxAt(const Vector2& position) const = 0;
	virtual bool HasExplosionAt(const Vector2& position) const = 0;
	virtual bool CanExplosionPenetrate(const Vector2& position) const = 0;
	virtual bool IsInsideGameMap(const Vector2& position) const = 0;
	virtual void SendItemToCharacter(const Vector2& itemPos, ItemType itemType) = 0;
};