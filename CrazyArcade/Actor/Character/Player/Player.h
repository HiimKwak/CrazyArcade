#pragma once

#include "Actor/Character/Character.h"
#include "Math/Vector2.h"

using namespace engine;

class Player : public Character
{
	RTTI_DECLARATIONS(Player, Character)

public:
	Player(const Vector2& position);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void OnDamaged();
	void OnKilled();
};
