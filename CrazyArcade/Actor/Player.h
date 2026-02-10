#pragma once

#include "CharacterBase.h"

class Bubble;

enum class PlayerState {
	NORMAL,
	TRAPPED_IN_BUBBLE,
	DEAD
};

class Player : public CharacterBase
{
	RTTI_DECLARATIONS(Player, CharacterBase)

public:
	Player(const Vector2& position);

	bool CanMove() const
	{
		return currentState == PlayerState::NORMAL || currentState == PlayerState::TRAPPED_IN_BUBBLE;
	}
	bool CanAct() const
	{
		return currentState == PlayerState::NORMAL;
	}

	inline bool IsDead() const { return currentState == PlayerState::DEAD; }
	inline PlayerState GetState() const { return currentState; }

	void OnDamaged();
	void OnKilled();

protected:
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	PlayerState currentState = PlayerState::NORMAL;
	void ChangeState(PlayerState newState);

private:
	void OnEnterBubbleTrap();
	void OnEnterDead();
	void OnEnterNormal();

	void TryMove(Vector2& direction);
	void UpdateStateTick(float deltaTime);
	void UpdateDeadTick(float deltaTime);
	void UpdateBubbleTrapTick(float deltaTime);
	void UpdateNormalTick(float deltaTime);
	void HandleMovementInput(float deltaTime);
	void HandleActionInput();
};
