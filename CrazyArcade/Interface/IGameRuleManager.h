#pragma once

#include "Math/Vector2.h"

using namespace engine;

class IGameRuleManager
{
public:
	virtual bool CanMove(
		const Vector2& from,
		const Vector2& to
	) = 0;
	virtual bool HasBubbleAt(const Vector2& position) = 0;
};