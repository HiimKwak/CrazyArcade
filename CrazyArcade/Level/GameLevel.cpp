#include "Game/Game.h"
#include "GameLevel.h"
#include "MenuLevel.h"
#include "Core/Input.h"
#include "Actor/Effects/WaterBalloon.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/State/StateComponent.h"
#include "Actor/Character/Component/Item/ItemComponent.h"
#include "Actor/Environments/Ground.h"
#include "Actor/Environments/Wall.h"
#include "Actor/Environments/Bush.h"
#include "Actor/Environments/Box.h"

#include "Engine/Engine.h"
#include "Math/Vector2.h"

using namespace engine;

GameLevel::GameLevel()
{
	worldQuery.BindActors(&actors);
	worldQuery.BindMapBounds(&mapOrigin, &mapWidth, &mapHeight);
	collisionSystem.BindActors(&actors);
	renderService.SetActors(&actors);
	renderService.SetTileSize(Engine::Get().GetTileSize());
	renderService.SetCurrentStage(1);

	mapLoader.SetHudWidth(RenderService::GetHudWidth());

	LoadMap("Stage1.txt");
}

GameLevel::GameLevel(int stage)
{
	worldQuery.BindActors(&actors);
	worldQuery.BindMapBounds(&mapOrigin, &mapWidth, &mapHeight);
	collisionSystem.BindActors(&actors);
	renderService.SetActors(&actors);
	renderService.SetTileSize(Engine::Get().GetTileSize());
	currentStage = stage;
	renderService.SetCurrentStage(stage);

	mapLoader.SetHudWidth(RenderService::GetHudWidth());

	char filename[32];
	sprintf_s(filename, 32, "Stage%d.txt", stage);
	LoadMap(filename);
}

GameLevel::~GameLevel()
{
}

void GameLevel::Tick(float deltaTime)
{
	if (renderService.IsPopupVisible())
	{
		if (Input::Get().GetKeyDown('S'))
			renderService.HidePopup();
		return;
	}

	super::Tick(deltaTime);

	const float blinkCycle = DEBUG_PATH_ON_TIME + DEBUG_PATH_OFF_TIME;
	debugPathBlinkElapsed += deltaTime;
	while (debugPathBlinkElapsed >= blinkCycle)
		debugPathBlinkElapsed -= blinkCycle;
	debugPathVisible = (debugPathBlinkElapsed < DEBUG_PATH_ON_TIME);

	if (CheckGameClear())
	{
		if (!isClearWaiting)
		{
			isClearWaiting = true;
			clearTimer.SetTargetTime(clearWaitTime);
			clearTimer.Reset();
		}

		clearTimer.Tick(deltaTime);

		if (Input::Get().GetKeyDown('N') || clearTimer.IsTimeout())
		{
			LoadNextMap();
			return;
		}

		if (Input::Get().GetKeyDown(VK_ESCAPE))
		{
			Engine::Get().SetNewLevel(new MenuLevel());
		}
		if (Input::Get().GetKeyDown('Q'))
		{
			Game::Get().Quit();
		}
		return;
	}

	if (CheckGameOver())
	{
		if (Input::Get().GetKeyDown(VK_ESCAPE))
		{
			Engine::Get().SetNewLevel(new MenuLevel());
		}
		if (Input::Get().GetKeyDown('Q'))
		{
			Game::Get().Quit();
		}
		return;
	}

	ProcessCollision();

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		Engine::Get().SetNewLevel(new MenuLevel());
		return;
	}

	if (Input::Get().GetKeyDown('Q'))
	{
		Game::Get().Quit();
	}
}

void GameLevel::ProcessCollision()
{
	collisionSystem.ProcessCollisions();
}

void GameLevel::Draw()
{
	super::Draw();

	renderService.SetDebugPathVisible(debugPathVisible);
	renderService.DrawDebugPathOverlay();

	Player* player = FindPlayer();
	renderService.DrawHud(player);
	renderService.DrawPopupOverlay();

	int remainingStages = MAX_STAGE - currentStage;
	float remainingTime = isClearWaiting ? clearTimer.GetRemainingTime() : 0.0f;
	renderService.DrawGameState(CheckGameOver(), CheckGameClear(), remainingStages, remainingTime);
}

void GameLevel::LoadMap(const char* filename)
{
	auto SpawnActorFn = [this](int character, const Vector2& position) {
		SpawnActor(character, position);
		};

	MapData mapData = mapLoader.LoadMap(filename, SpawnActorFn);

	mapWidth = mapData.width;
	mapHeight = mapData.height;
	mapOrigin = mapData.origin;

	mapLoader.SpawnBoundaryWalls(mapData, SpawnActorFn);
}

void GameLevel::SpawnActor(int character, const Vector2& position)
{
	switch (character)
	{
	case '0':
		AddNewActor(new Ground(position));
		break;
	case '1':
		AddNewActor(new Wall(position));
		break;
	case '2':
		AddNewActor(new Box(position));
		AddNewActor(new Ground(position));
		break;
	case '3':
	{
		Player* player = new Player(position);
		player->SetDelegate(this);
		player->SetQueryDelegate(&worldQuery);
		auto itemComp = player->GetComponent<ItemComponent>();
		if (itemComp)
		{
			itemComp->SetOnItemAcquiredCallback([this, shieldPopupShown = false](ItemType type) mutable
				{
					if (type != ItemType::Shield || shieldPopupShown)
						return;

					shieldPopupShown = true;
					renderService.ShowShieldPopup();
				});
		}
		AddNewActor(player);
		AddNewActor(new Ground(position));
		break;
	}
	case '4':
	{
		Enemy* enemy = new Enemy(position);
		enemy->SetDelegate(this);
		enemy->SetQueryDelegate(&worldQuery);
		AddNewActor(enemy);
		AddNewActor(new Ground(position));
		break;
	}
	case '5':
		AddNewActor(new Bush(position));
		break;
	case 'W': // Boundary wall
		AddNewActor(new Wall(position, false));
		break;
	}
}

void GameLevel::LoadNextMap()
{
	if (currentStage >= 3)
	{
		Game::Get().ToggleMenu();
	}
	else
	{
		Engine::Get().SetNewLevel(new GameLevel(currentStage + 1));
	}
}


bool GameLevel::CanMove(const Vector2& currentPos, const Vector2& nextPos)
{
	Actor* targetActor = nullptr;
	for (Actor* actor : actors)
	{
		if (actor->GetPosition() == nextPos)
		{
			targetActor = actor;
			break;
		}
	}

	if (targetActor == nullptr)
		return true;

	if (targetActor->IsTypeOf<Wall>() || targetActor->IsTypeOf<Box>())
		return false;

	if (targetActor->IsTypeOf<WaterBalloon>())
	{
		const int ts = Engine::Get().GetTileSize();
		Vector2 currentTile = Vector2((currentPos.x / ts) * ts, (currentPos.y / ts) * ts);
		Vector2 balloonTile = Vector2((nextPos.x / ts) * ts, (nextPos.y / ts) * ts);

		if (currentTile != balloonTile)
		{
			Vector2 direction = nextPos - currentPos;
			Vector2 balloonNextPos = nextPos + direction;

			for (Actor* actor : actors)
			{
				if (actor->GetPosition() == balloonNextPos)
				{
					if (actor->IsTypeOf<Wall>() ||
						actor->IsTypeOf<Box>() ||
						actor->IsTypeOf<WaterBalloon>() ||
						actor->IsTypeOf<Player>() ||
						actor->IsTypeOf<Enemy>())
					{
						return false;
					}
				}
			}
		}
		return true;
	}

	return true;
}

bool GameLevel::Push(const Vector2& pusherPos, const Vector2& targetPos)
{
	Actor* pusher = nullptr;
	Actor* target = nullptr;

	for (Actor* actor : actors)
	{
		if (actor->GetPosition() == pusherPos)
			pusher = actor;
		if (actor->GetPosition() == targetPos)
			target = actor;
	}

	if (!pusher || !target)
		return false;

	Vector2 direction = targetPos - pusherPos;
	Vector2 targetNextPos = targetPos + direction;

	for (Actor* actor : actors)
	{
		if (actor->GetPosition() == targetNextPos)
		{
			if (actor->IsTypeOf<Wall>() ||
				actor->IsTypeOf<Box>() ||
				actor->IsTypeOf<WaterBalloon>() ||
				actor->IsTypeOf<Player>() ||
				actor->IsTypeOf<Enemy>())
			{
				return false;
			}
		}
	}

	target->SetPosition(targetNextPos);
	pusher->SetPosition(targetPos);

	return true;
}

bool GameLevel::CheckGameClear()
{
	// prioritize 'game over' over 'game clear' 
	Player* player = FindPlayer();
	if (player)
	{
		auto playerState = player->GetComponent<StateComponent>();
		if (playerState && playerState->IsDead())
			return false;
	}

	std::vector<Actor*> aliveEnemies;

	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Enemy>())
		{
			Enemy* enemy = actor->As<Enemy>();
			auto stateComp = enemy->GetComponent<StateComponent>();
			if (stateComp && !stateComp->IsDead())
				aliveEnemies.emplace_back(actor);
		}
	}

	return aliveEnemies.size() == 0;
}

bool GameLevel::CheckGameOver()
{
	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Player>())
		{
		 Player* player = actor->As<Player>();
			auto stateComp = player->GetComponent<StateComponent>();
			if (stateComp && !stateComp->IsDead())
				return false;
		}
	}
	return true;
}

bool GameLevel::OnRequestMove(Character* character, const Vector2& targetPos)
{
	Vector2 currentPos = character->GetPosition();

	if (!CanMove(currentPos, targetPos))
		return false;

	auto stateComp = character->GetComponent<StateComponent>();
	if (stateComp && stateComp->GetCurrentState() == StateType::Normal && worldQuery.HasActorAt(targetPos, ActorType::WaterBalloon))
	{
		if (!Push(currentPos, targetPos))
			return false;
	}

	return true;
}

bool GameLevel::OnRequestGenerateWaterBalloon(Character* character, const Vector2& position, int range)
{
	if (worldQuery.HasActorAt(position, ActorType::WaterBalloon))
		return false;

	WaterBalloon* waterBalloon = new WaterBalloon(position, range);
	waterBalloon->SetOwnerCharacter(character);
	AddNewActor(waterBalloon);
	return true;
}

bool GameLevel::OnQueryCanMove(const Vector2& from, const Vector2& to)
{
	return CanMove(from, to);
}

Player* GameLevel::FindPlayer() const
{
	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Player>())
			return actor->As<Player>();
	}
	return nullptr;
}
