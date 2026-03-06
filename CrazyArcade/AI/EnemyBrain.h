#pragma once

#include "Math/Vector2.h"

using namespace engine;

class Character;

class EnemyBrain
{
public:
	struct Settings
	{
		float thinkIntervalMin = 0.15f;
		float thinkIntervalMax = 0.45f;
		float idleChance = 0.15f;
		float chaseChance = 0.70f;
		int chaseLookahead = 10;
	};

	explicit EnemyBrain(Settings settings = {})
		: settings(settings)
	{
	}

	Vector2 ChooseDirection(const Vector2& enemyPos, const Vector2& playerPos, Character* character);

	const Settings& GetSettings() const { return settings; }

private:
	Settings settings;

	Vector2 PickRandomValidDirection(const Vector2& enemyPos, Character* character);
	Vector2 PickChaseDirection(const Vector2& enemyPos, const Vector2& playerPos, Character* character);
	bool IsDirectionValid(const Vector2& enemyPos, const Vector2& dir, Character* character) const;
};
