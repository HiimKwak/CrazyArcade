#pragma once
#include "Level/Level.h"
#include "Common/Common.h"
#include "Interface/IWorldQueryProvider.h"
#include "Actor/Character/Character.h"
#include "Util/Timer.h"
#include "Level/Query/WorldQueryService.h"
#include "Level/Collision/ActorCollisionSystem.h"
#include "Level/Render/RenderService.h"
#include "Level/Loader/GameMapLoader.h"

using namespace engine;

class GameLevel : public engine::Level, public IWorldQueryProvider, public CharacterCommandDelegate
{
	RTTI_DECLARATIONS(GameLevel, engine::Level)

public:
	GameLevel();
	GameLevel(int stage);
	~GameLevel();

	virtual void Draw() override;

	inline Vector2 GetMapOrigin() const { return mapOrigin; }

	IWorldQueryService* GetWorldQueryService() override { return &worldQuery; }

private:
	virtual void Tick(float deltaTime) override;
	void ProcessCollision();

	void LoadMap(const char* filename);
	void LoadNextMap();
	void SpawnActor(int character, const Vector2& position);

	bool CanMove(const Vector2& currentPos, const Vector2& nextPos);
	bool Push(const Vector2& pusherPos, const Vector2& targetPos);

	// CharacterCommandDelegate
	virtual bool OnRequestMove(Character* character, const Vector2& targetPos) override;
	virtual bool OnRequestGenerateWaterBalloon(Character* character, const Vector2& position, int range) override;
	virtual bool OnQueryCanMove(const Vector2& from, const Vector2& to) override;

	bool CheckGameClear();
	bool CheckGameOver();

	Player* FindPlayer() const;

private:
	Vector2 mapOrigin = Vector2::Zero;

	int mapWidth = 0;
	int mapHeight = 0;

	int currentStage = 1;
	static constexpr int MAX_STAGE = 3;
	bool isClearWaiting = false;
	Timer clearTimer;
	float clearWaitTime = 5.0f;
	float debugPathBlinkElapsed = 0.0f;
	bool  debugPathVisible = false;
	static constexpr float DEBUG_PATH_ON_TIME = 0.2f;
	static constexpr float DEBUG_PATH_OFF_TIME = 1.2f;

	WorldQueryService worldQuery;
	ActorCollisionSystem collisionSystem;
	RenderService renderService;
	GameMapLoader mapLoader;
};

