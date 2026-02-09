#pragma once
#include "Level/Level.h"
#include "Common/Common.h"
#include "Interface/IGameRuleManager.h"

using namespace engine;

class GameLevel : public Level, public IGameRuleManager
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	~GameLevel();

	virtual void Draw() override;

private:
	virtual void Tick(float deltaTime) override;
	void ProcessExplosionCollision();

	void LoadMap(const char* filename);

	virtual bool CanMove(const Vector2& currentPos, const Vector2& nextPos) override;
	virtual bool CanExplosionPenetrate(const Vector2& position) override;

	virtual bool Push(const Vector2& pusherPos, const Vector2& targetPos) override;
	
	virtual bool HasBubbleAt(const Vector2& position) override;
	virtual bool HasPlayerAt(const Vector2& position) override;
	virtual bool HasExplosionAt(const Vector2& position) override;
	virtual bool HasBoxAt(const Vector2& position) override;

	virtual void SendItemToPlayer(const Vector2& itemPos, ItemType itemType) override;

	bool CheckGameClear();
	bool CheckGameOver();

private:
	int targetScore = 0;
};

