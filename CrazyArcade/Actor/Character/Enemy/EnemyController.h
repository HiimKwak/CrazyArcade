#pragma once

#include "Actor/Character/Character.h"
#include "Math/Vector2.h"

using namespace engine;

class EnemyBrain;

class EnemyController
{
public:
	EnemyController(class Enemy* owner);
	~EnemyController();

	void Tick(float deltaTime);

private:
	void HandleMovement(float deltaTime);
	void HandleAction(float deltaTime);

	class Enemy* owner;
	EnemyBrain* brain;

	float thinkTimer = 0.0f;
	float shootTimer = 0.0f;
	Vector2 desiredDirection = Vector2::Zero;
};
