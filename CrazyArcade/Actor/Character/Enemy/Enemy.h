#pragma once

#include "Actor/Character/Character.h"
#include "Math/Vector2.h"
#include <vector>

using namespace engine;

class Enemy : public Character
{
	RTTI_DECLARATIONS(Enemy, Character)

public:
	Enemy(const Vector2& position);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void OnDamaged();

	const std::vector<Vector2>& GetDebugPath() const { return debugPath; }
	void SetDebugPath(std::vector<Vector2> path) { debugPath = std::move(path); }

private:
	std::vector<Vector2> debugPath;
};
