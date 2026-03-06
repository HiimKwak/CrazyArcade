#pragma once

#include "Actor/Character/Character.h"
#include "Math/Vector2.h"

using namespace engine;

class EnemyController;

class Enemy : public Character
{
	RTTI_DECLARATIONS(Enemy, Character)

public:
	Enemy(const Vector2& position);
	~Enemy();

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void OnDamaged();

private:
	EnemyController* controller;
};
