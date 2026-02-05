#pragma once

#include "Math/Vector2.h"

class IGameRuleManager
{
public:
	virtual bool CanMove(
		const engine::Vector2& from,
		const engine::Vector2& to
	) = 0;
	virtual bool HasBubbleAt(const Vector2& position) = 0;
};