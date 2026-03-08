#pragma once
#include "Level/Level.h"
#include "Common/Common.h"
#include "Interface/IGameRuleManager.h"
#include "Actor/Character/Character.h"
#include "Util/Timer.h"

using namespace engine;

class GameLevel : public Level, public IGameRuleManager, public CharacterDelegate
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	GameLevel(int stage);
	~GameLevel();

	virtual void Draw() override;

	Vector2 WorldToScreen(const Vector2& worldPos) const;
	bool IsInsideGameMap_Screen(const Vector2& worldPos) const;
	bool IsInsideGameMap_World(const Vector2& worldPos) const;
	inline Vector2 GetMapOrigin() const { return mapOrigin; }

private:
	virtual void Tick(float deltaTime) override;
	void ProcessCollision();

	void LoadMap(const char* filename);
	void LoadNextMap();

	// IGameRuleManager (used by Item, BubbleExplosion, etc.)
	virtual bool CanMove(const Vector2& currentPos, const Vector2& nextPos) override;
	virtual bool CanExplosionPenetrate(const Vector2& position) override;

	virtual bool Push(const Vector2& pusherPos, const Vector2& targetPos) override;

	virtual bool HasBubbleAt(const Vector2& position) override;
	virtual bool HasPlayerAt(const Vector2& position) override;
	virtual bool HasExplosionAt(const Vector2& position) override;
	virtual bool HasBoxAt(const Vector2& position) override;

	virtual void SendItemToPlayer(const Vector2& itemPos, ItemType itemType) override;

	virtual Vector2 GetPlayerPosition() override;

	// CharacterDelegate
	virtual bool OnRequestMove(Character* character, const Vector2& targetPos) override;
	virtual bool OnRequestGenerateBubble(Character* character, const Vector2& position, int range) override;
	virtual bool OnQueryCanMove(const Vector2& from, const Vector2& to) override;
	virtual Vector2 OnQueryPlayerPosition() override;
	virtual bool OnQueryHasBubbleAt(const Vector2& position) override;
	virtual bool OnQueryHasBoxAt(const Vector2& position) override;
	virtual bool OnQueryIsExplosionDangerAt(const Vector2& position) override;

	bool CheckGameClear();
	bool CheckGameOver();

private:
	Vector2 mapOrigin = Vector2::Zero;
	Vector2 screenCenter;

	int mapWidth = 0;
	int mapHeight = 0;

	int targetScore = 0;

	int currentStage = 1;
	bool isClearWaiting = false;
	Timer clearTimer;
	float clearWaitTime = 10.0f;
};

