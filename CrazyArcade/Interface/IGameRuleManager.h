#pragma once

#include "Math/Vector2.h"
#include "Actor/Item.h"

using namespace engine;

class IGameRuleManager
{
public:
	virtual bool CanMove(
		const Vector2& currentPos,
		const Vector2& nextPos
	) = 0;
	virtual bool CanExplosionPenetrate(const Vector2& position) = 0;

	virtual bool Push(const Vector2& pusherPos, const Vector2& targetPos) = 0;

	virtual bool HasBubbleAt(const Vector2& position) = 0;
	virtual bool HasPlayerAt(const Vector2& position) = 0;
	virtual bool HasExplosionAt(const Vector2& position) = 0;
	virtual bool HasBoxAt(const Vector2& position) = 0;

	virtual void SendItemToPlayer(const Vector2& itemPos, ItemType itemType) = 0; // 좌표 판별로는 아이템 획득 순서 보장을 완벽히 해주지 못함

	virtual Vector2 GetPlayerPosition() = 0;
};