#pragma once
#include "Level/Level.h"
#include "Common/Common.h"
#include "Interface/IGameRuleManager.h"

using namespace engine;

class GameLevel : public engine::Level, public IGameRuleManager
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();

	virtual void Draw() override;

private:
	virtual void Tick(float deltaTime) override;
	void ProcessExplosionCollision();

	void LoadMap(const char* filename);

	virtual bool CanMove(const engine::Vector2& playerPosition, const engine::Vector2& nextPosition) override;
	virtual bool HasBubbleAt(const engine::Vector2& position) override;

	bool CheckGameClear();
private:
	int targetScore = 0;
	bool isGameClear = false;
};

