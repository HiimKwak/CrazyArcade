#pragma once
#include "Level/Level.h"
#include "Common/Common.h"
#include "Interface/IWorldQueryProvider.h"
#include "Actor/Character/Character.h"
#include "Util/Timer.h"
#include "Level/Query/WorldQueryService.h"
#include "Level/Collision/ActorCollisionSystem.h"
#include "Level/Render/GameRenderService.h"

using namespace engine;

class GameLevel : public Level, public IWorldQueryProvider, public CharacterCommandDelegate
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

	IWorldQueryService* GetWorldQueryService() override { return &worldQuery; }

private:
	virtual void Tick(float deltaTime) override;
	void ProcessCollision();

	void LoadMap(const char* filename);
	void LoadNextMap();

	bool CanMove(const Vector2& currentPos, const Vector2& nextPos);
	bool CanExplosionPenetrate(const Vector2& position);

	bool Push(const Vector2& pusherPos, const Vector2& targetPos);

	void SendItemToPlayer(const Vector2& itemPos, ItemType itemType);

	Vector2 GetActorPosition(ActorType type);

	// CharacterCommandDelegate
	virtual bool OnRequestMove(Character* character, const Vector2& targetPos) override;
	virtual bool OnRequestGenerateBubble(Character* character, const Vector2& position, int range) override;
	virtual bool OnQueryCanMove(const Vector2& from, const Vector2& to) override;

	bool CheckGameClear();
	bool CheckGameOver();

	void HandleCommonHotkeys();

	Player* FindPlayer() const;

	void CollectCollisionTargets();

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
	float debugPathBlinkElapsed = 0.0f;
	bool  debugPathVisible = false;
	static constexpr float DEBUG_PATH_ON_TIME = 0.2f;
	static constexpr float DEBUG_PATH_OFF_TIME = 1.2f;

	WorldQueryService worldQuery;
	ActorCollisionSystem collisionSystem;
	GameRenderService renderService;
};

