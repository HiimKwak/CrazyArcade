#pragma once

#include "Actor/CharacterBase.h"

class EnemyBrain;

enum class EnemyState {
	NORMAL,
	TRAPPED_IN_BUBBLE,
	DEAD
};

class Enemy : public CharacterBase
{
	RTTI_DECLARATIONS(Enemy, CharacterBase)

public:
	Enemy(const Vector2& position);
	~Enemy();

	bool CanMove() const
	{
		return currentState == EnemyState::NORMAL || currentState == EnemyState::TRAPPED_IN_BUBBLE;
	}
	bool CanAct() const
	{
		return currentState == EnemyState::NORMAL;
	}

	inline bool IsDead() const { return currentState == EnemyState::DEAD; }
	inline EnemyState GetState() const { return currentState; }

	void OnDamaged();

protected:
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	EnemyState currentState = EnemyState::NORMAL;
	void ChangeState(EnemyState newState);

private:
	void OnEnterBubbleTrap();
	void OnEnterDead();
	void OnEnterNormal();

	void TryStartMove(const Vector2& direction);
	void UpdateStateTick(float deltaTime);
	void UpdateDeadTick(float deltaTime);
	void UpdateBubbleTrapTick(float deltaTime);
	void UpdateNormalTick(float deltaTime);
	void HandleMovement(float deltaTime);
	void HandleAction(float deltaTime);

	Timer thinkTimer;
	EnemyBrain* brain = nullptr;
	Vector2 desiredDirection = Vector2::Zero;

	Timer shootTimer;
};
