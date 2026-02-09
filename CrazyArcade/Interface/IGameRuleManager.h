#pragma once

#include "Math/Vector2.h"

using namespace engine;

class IGameRuleManager
{
public:
	virtual bool CanMove(
		const Vector2& currentPos,
		const Vector2& nextPos
	) = 0;
	virtual bool Push(const Vector2& pusherPos, const Vector2& targetPos) = 0;
	virtual bool HasBubbleAt(const Vector2& position) = 0;
	virtual bool CanExplosionPenetrate(const Vector2& position) = 0;
};