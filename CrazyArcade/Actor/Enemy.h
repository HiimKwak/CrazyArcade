#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"
#include "Interface/IGameRuleManager.h"

using namespace engine;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor)

public:
	Enemy(const Vector2& position);
	~Enemy();

	virtual void Tick(float deltaTime) override;

	void OnDamaged();

private:
	Vector2 direction = Vector2::Zero;
	float moveIntervalMin = 0.25f;
	float moveIntervalMax = 0.6f;
	float moveSpeed = 1.0f;

	Timer moveTimer;
	Timer shootTimer;

};
