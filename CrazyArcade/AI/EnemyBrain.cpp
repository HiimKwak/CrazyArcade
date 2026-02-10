#include "EnemyBrain.h"

#include "Interface/IGameRuleManager.h"
#include "Util/Util.h"
#include "Math/Vector2.h"

namespace
{
	Vector2 Directions[4] = { Vector2::Right, Vector2::Left, Vector2::Up, Vector2::Down };
}

Vector2 EnemyBrain::ChooseDirection(const Vector2& enemyPos, const Vector2& playerPos, IGameRuleManager* rules)
{
	if (!rules)
		return Vector2::Zero;

	// occasionally idle
	if (Util::RandomRange(0.0f, 1.0f) < settings.idleChance)
		return Vector2::Zero;

	// chase most of the time
	if (Util::RandomRange(0.0f, 1.0f) < settings.chaseChance)
	{
		Vector2 chaseDir = PickChaseDirection(enemyPos, playerPos, rules);
		if (chaseDir != Vector2::Zero)
			return chaseDir;
	}

	return PickRandomValidDirection(enemyPos, rules);
}

Vector2 EnemyBrain::PickRandomValidDirection(const Vector2& enemyPos, IGameRuleManager* rules)
{
	// shuffle-ish: try up to 4 random picks
	for (int i = 0; i < 4; ++i)
	{
		int r = Util::Random(0, 4);
		Vector2 dir = Directions[r];
		if (IsDirectionValid(enemyPos, dir, rules))
			return dir;
	}

	// fallback: deterministic scan
	for (Vector2 dir : Directions)
	{
		if (IsDirectionValid(enemyPos, dir, rules))
			return dir;
	}

	return Vector2::Zero;
}

Vector2 EnemyBrain::PickChaseDirection(const Vector2& enemyPos, const Vector2& playerPos, IGameRuleManager* rules)
{
	Vector2 delta = playerPos - enemyPos;

	Vector2 primary;
	Vector2 secondary;

	if (abs(static_cast<int>(delta.x)) >= abs(static_cast<int>(delta.y)))
	{
		primary = (delta.x >= 0) ? Vector2::Right : Vector2::Left;
		secondary = (delta.y >= 0) ? Vector2::Down : Vector2::Up;
	}
	else
	{
		primary = (delta.y >= 0) ? Vector2::Down : Vector2::Up;
		secondary = (delta.x >= 0) ? Vector2::Right : Vector2::Left;
	}

	if (IsDirectionValid(enemyPos, primary, rules))
		return primary;
	if (IsDirectionValid(enemyPos, secondary, rules))
		return secondary;

	return PickRandomValidDirection(enemyPos, rules);
}

bool EnemyBrain::IsDirectionValid(const Vector2& enemyPos, const Vector2& dir, IGameRuleManager* rules) const
{
	if (dir == Vector2::Zero)
		return false;

	Vector2 next = enemyPos + dir;
	return rules->CanMove(enemyPos, next);
}
