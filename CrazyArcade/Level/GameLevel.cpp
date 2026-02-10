#include "Game/Game.h"
#include "GameLevel.h"
#include "Core/Input.h"
#include "Util/Util.h"
#include "Actor/Bubble.h"
#include "Actor/Ground.h"
#include "Actor/Player.h"
#include "Actor/Enemy.h"
#include "Actor/Wall.h"
#include "Actor/Bush.h"
#include "Actor/ExplosionTile.h"
#include "Actor/Box.h"
#include "Actor/Item.h"

#include "Engine/Engine.h"
#include "Math/Vector2.h"
#include "Render/Renderer.h"

#include <iostream>	
#include <Windows.h>
#include <string>

using namespace engine;

Vector2 GameLevel::WorldToScreen(const Vector2& worldPos) const
{
	return mapOrigin + worldPos;
}

bool GameLevel::IsInsideGameMap_Screen(const Vector2& screenPos) const
{
	if (mapWidth <= 0 || mapHeight <= 0)
		return false;

	const int left = mapOrigin.x;
	const int top = mapOrigin.y;
	const int right = mapOrigin.x + (mapWidth - 1);
	const int bottom = mapOrigin.y + (mapHeight - 1);

	return (screenPos.x >= left && screenPos.x <= right &&
		screenPos.y >= top && screenPos.y <= bottom);
}

bool GameLevel::IsInsideGameMap_World(const Vector2& worldPos) const
{
	return IsInsideGameMap_Screen(WorldToScreen(worldPos));
}


GameLevel::GameLevel()
{
	LoadMap("Stage1.txt");
}

GameLevel::GameLevel(int stage)
{
	currentStage = stage;

	char filename[32];
	sprintf_s(filename, 32, "Stage%d.txt", stage);
	LoadMap(filename);
}

GameLevel::~GameLevel()
{
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 게임 오버 처리
	if (CheckGameOver())
	{
		if (Input::Get().GetKeyDown(VK_ESCAPE))
		{
			Game::Get().ToggleMenu();
		}
		if (Input::Get().GetKeyDown('Q'))
		{
			Game::Get().Quit();
		}
		return;
	}

	// 게임 클리어 처리
	if (CheckGameClear())
	{
		if (!isClearWaiting)
		{
			isClearWaiting = true;
			clearTimer.SetTargetTime(clearWaitTime);
			clearTimer.Reset();
		}

		clearTimer.Tick(deltaTime);

		// N키를 누르거나 10초가 지나면 다음 스테이지로
		if (Input::Get().GetKeyDown('N') || clearTimer.IsTimeout())
		{
			LoadNextMap();
			return;
		}

		if (Input::Get().GetKeyDown(VK_ESCAPE))
		{
			Game::Get().ToggleMenu();
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
		Game::Get().ToggleMenu();
		return;
	}

	if (Input::Get().GetKeyDown('Q'))
	{
		Game::Get().Quit();
	}
}

void GameLevel::ProcessCollision()
{
	std::vector<ExplosionTile*> activeExplosions;
	Player* player = nullptr;
	std::vector<Enemy*> enemies;
	std::vector<Box*> boxes;

	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<ExplosionTile>())
		{
			ExplosionTile* seg = actor->As<ExplosionTile>();
			if (seg->IsActive())
				activeExplosions.push_back(seg);
		}

		if (actor->IsTypeOf<Player>())
			player = actor->As<Player>();

		if (actor->IsTypeOf<Enemy>())
			enemies.push_back(actor->As<Enemy>());

		if (actor->IsTypeOf<Box>())
			boxes.push_back(actor->As<Box>());
	}

	for (ExplosionTile* explosion : activeExplosions)
	{
		Vector2 explosionPos = explosion->GetPosition();

		if (player && player->GetState() == PlayerState::NORMAL && player->GetPosition() == explosionPos)
			player->OnDamaged();

		for (Enemy* enemy : enemies)
		{
			if (enemy->GetPosition() == explosionPos)
				enemy->OnDamaged();
		}

		for (Box* box : boxes)
		{
			if (box->GetPosition() == explosionPos)
				box->OnDamaged();
		}
	}

	for (Enemy* enemy : enemies)
	{
		if (player && !player->IsDead() &&
			!enemy->IsDead() &&
			enemy->GetPosition() == player->GetPosition())
		{
			player->OnKilled();
		}
	}
}

void GameLevel::Draw()
{
	super::Draw();


	Player* player = nullptr;
	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Player>())
		{
			player = actor->As<Player>();
			break;
		}
	}
	const int gameH = Engine::Get().GetGameHeight();

	wchar_t stageStr[32] = {};
	swprintf_s(stageStr, 32, L"Stage: %d", currentStage);
	Renderer::Get().Submit(stageStr, Vector2(0, 0), Color::White, 100);

	Renderer::Get().Submit(L"---------------------------------------------", Vector2(0, gameH), Color::White, 100);
	if (player)
	{
		wchar_t hpStr[128] = {};
		swprintf_s(hpStr, 128, L"HP: %d", player->GetLives());
		Renderer::Get().Submit(hpStr, Vector2(0, gameH + 1), Color::Red, 100);

		wchar_t bubbleStr[1024] = {};
		swprintf_s(bubbleStr, 1024, L"Bubble: %d/%d  Range: %d",
			player->GetBubbleAmmo(),
			player->GetMaxBubbleAmmo(),
			player->GetBubbleRange());
		Renderer::Get().Submit(bubbleStr, Vector2(17, gameH + 1), Color::Yellow, 100);
	}

	Renderer::Get().Submit(L"ESC: Menu   Q: Quit", Vector2(0, gameH + 4), Color::Orange, 100);

	if (CheckGameOver())
	{
		Renderer::Get().Submit(L"Game Over!", Vector2(0, gameH + 3), Color::Red, 100);
	}

	if (CheckGameClear())
	{
		Renderer::Get().Submit(L"Game Clear!", Vector2(0, gameH + 3), Color::White, 100);
	}
}

void GameLevel::LoadMap(const char* filename)
{
	/** 파일 불러오기 **/
	char path[2048] = {};
	sprintf_s(path, 2048, "../Assets/%s", filename);

	FILE* file = nullptr;
	fopen_s(&file, path, "rb");

	if (!file)
	{
		std::cerr << "Failed to open map file.\n";
		__debugbreak();
	}

	/** 버퍼에 담기 **/
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	char* data = new char[fileSize + 1];
	size_t readSize = fread(data, sizeof(char), fileSize, file);
	data[fileSize] = '\0';

	// 1) 맵 크기 산출(mapWidth/mapHeight)
	int computedMapWidth = 0;
	int computedMapHeight = 0;
	int currentLineWidth = 0;
	bool hasContent = false;

	for (size_t i = 0; i < fileSize; ++i)
	{
		if (data[i] == '\r')
			continue;

		if (data[i] == '\n')
		{
			if (currentLineWidth > 0)
			{
				if (currentLineWidth > computedMapWidth) computedMapWidth = currentLineWidth;
				computedMapHeight++;
				currentLineWidth = 0;
			}
			continue;
		}
		++currentLineWidth;
		hasContent = true;
	}
	if (currentLineWidth > 0)
	{
		if (currentLineWidth > computedMapWidth) computedMapWidth = currentLineWidth;
		computedMapHeight++;
	}

	mapWidth = computedMapWidth;
	mapHeight = computedMapHeight;

	const int gameW = Engine::Get().GetGameWidth();
	const int gameH = Engine::Get().GetGameHeight();

	const Vector2 gameCenter((gameW - 1) / 2, (gameH - 1) / 2);
	const Vector2 mapHalf(mapWidth / 2, mapHeight / 2);

	mapOrigin = gameCenter - mapHalf;
	screenCenter = gameCenter;

	/** 읽어온 문자열 분석(parsing) **/
	int index = 0;
	Vector2 worldPos = Vector2::Zero;

	while (index < (int)fileSize)
	{
		char mapCharacter = data[index];
		++index;

		if (mapCharacter == '\r')
			continue;

		if (mapCharacter == '\n')
		{
			++worldPos.y;
			worldPos.x = 0;
			continue;
		}

		Vector2 screenPos = WorldToScreen(worldPos);

		switch (mapCharacter)
		{
		case '0':
			AddNewActor(new Ground(screenPos));
			break;
		case '1':
			AddNewActor(new Wall(screenPos));
			break;
		case '2':
			AddNewActor(new Box(screenPos));
			AddNewActor(new Ground(screenPos));
			break;
		case '3':
			AddNewActor(new Player(screenPos));
			AddNewActor(new Ground(screenPos));
			break;
		case '4':
			AddNewActor(new Enemy(screenPos));
			AddNewActor(new Ground(screenPos));
			break;
		case '5':
			AddNewActor(new Bush(screenPos));
			break;
		}

		++worldPos.x;
	}

	delete[] data;
	fclose(file);

	// 맵 테두리 생성
	for (int x = -1; x <= mapWidth; ++x)
	{
		Vector2 fenceUp = WorldToScreen(Vector2(x, -1));
		AddNewActor(new Wall(fenceUp, false));
		Vector2 fenceDown = WorldToScreen(Vector2(x, mapHeight));
		AddNewActor(new Wall(fenceDown, false));
	}


	for (int y = 0; y < mapHeight; ++y)
	{
		Vector2 fenceLeft = WorldToScreen(Vector2(-1, y));
		AddNewActor(new Wall(fenceLeft, false));
		Vector2 fenceRight = WorldToScreen(Vector2(mapWidth, y));
		AddNewActor(new Wall(fenceRight, false));
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

	if (targetActor->IsTypeOf<Bubble>())
	{
		Vector2 direction = nextPos - currentPos;
		Vector2 bubbleNextPos = nextPos + direction;

		for (Actor* actor : actors)
		{
			if (actor->GetPosition() == bubbleNextPos)
			{
				if (actor->IsTypeOf<Wall>() ||
					actor->IsTypeOf<Box>() ||
					actor->IsTypeOf<Bubble>() ||
					actor->IsTypeOf<Player>() ||
					actor->IsTypeOf<Enemy>())
				{
					return false;
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
				actor->IsTypeOf<Bubble>() ||
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

bool GameLevel::HasBubbleAt(const Vector2& position)
{
	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Bubble>() && actor->GetPosition() == position)
			return true;
	}
	return false;
}

bool GameLevel::HasPlayerAt(const Vector2& position)
{
	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Player>() && actor->GetPosition() == position)
			return true;
	}
	return false;
}

bool GameLevel::HasExplosionAt(const Vector2& position)
{
	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<ExplosionTile>() && actor->GetPosition() == position)
			return true;
	}
	return false;
}

bool GameLevel::HasBoxAt(const Vector2& position)
{
	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Box>() && actor->GetPosition() == position)
			return true;
	}
	return false;
}

void GameLevel::SendItemToPlayer(const Vector2& itemPos, ItemType itemType)
{
	for (Actor* actor : actors)
	{
		// todo: 좌표 판별로는 아이템 획득 순서 보장을 완벽히 해주지 못함
		if (actor->IsTypeOf<Player>() && actor->GetPosition() == itemPos)
		{
			Player* player = actor->As<Player>();
			player->AddItem(itemType);
			break;
		}
	}
}

Vector2 GameLevel::GetPlayerPosition()
{
	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Player>())
		{
			Player* player = actor->As<Player>();
			return player->GetPosition();
		}
	}
	return Vector2::Zero; // 플레이어가 없을 경우?
}

bool GameLevel::CanExplosionPenetrate(const Vector2& position)
{
	for (Actor* actor : actors)
	{
		if (actor->GetPosition() == position && (actor->IsTypeOf<Wall>() || actor->IsTypeOf<Bubble>()))
			return false;
	}
	return true;
}

bool GameLevel::CheckGameClear()
{
	std::vector<Actor*> aliveEnemies;

	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Enemy>())
		{
			Enemy* enemy = actor->As<Enemy>();
			if (!enemy->IsDead()) 
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
			if (!player->IsDead())
				return false;
		}
	}
	return true;
}
