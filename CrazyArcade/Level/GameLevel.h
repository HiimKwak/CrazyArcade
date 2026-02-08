#pragma once
#include "Level/Level.h"
#include "Common/Common.h"
#include "Interface/IGameRuleManager.h"

using namespace engine;

class ExplosionTilePool;

class GameLevel : public Level, public IGameRuleManager
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	~GameLevel();

	virtual void Draw() override;

	inline ExplosionTilePool* GetExplosionTilePool() const { return explosionPool; }

private:
	virtual void Tick(float deltaTime) override;
	void ProcessExplosionCollision();

	void LoadMap(const char* filename);

	virtual bool CanMove(const Vector2& playerPosition, const Vector2& nextPosition) override;
	virtual bool HasBubbleAt(const Vector2& position) override;

	bool CheckGameClear();
private:
	int targetScore = 0;
	bool isGameClear = false;
	bool isGameOver = false;
	ExplosionTilePool* explosionPool = nullptr;
};

