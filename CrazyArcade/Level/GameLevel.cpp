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

GameLevel::~GameLevel()
{
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

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
			player->OnDamaged();
		}
	}
}

void GameLevel::Draw()
{
	super::Draw();

	const int gameH = Engine::Get().GetGameHeight();

	Renderer::Get().Submit(L"---------------------------------------------", Vector2(0, gameH), Color::White, 100);
	Renderer::Get().Submit(L"ESC: Menu   Q: Quit", Vector2(0, gameH + 1), Color::White, 100);

	if (CheckGameClear())
	{
		Util::SetConsolePosition(Vector2(30, 0));
		Util::SetConsoleTextColor(Color::White);

		std::cout << "Game Clear!";
	}
}

void GameLevel::LoadMap(const char* filename)
{
	/** 파일 불러오기 **/
	char path[2048] = {};
	sprintf_s(path, 2048, "../Assets/%s", filename);

	FILE* file = nullptr;
	fopen_s(&file, path, "rt");

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
	int computedMapHeight = 1;
	int currentLineWidth = 0;

	for (size_t i = 0; i < fileSize; ++i)
	{
		if (data[i] == '\r')
			continue;

		if (data[i] == '\n')
		{
			if (currentLineWidth > computedMapWidth) computedMapWidth = currentLineWidth;
			currentLineWidth = 0;
			++computedMapHeight;
			continue;
		}
		++currentLineWidth;
	}
	if (currentLineWidth > computedMapWidth) computedMapWidth = currentLineWidth;

	mapWidth = computedMapWidth;
	mapHeight = computedMapHeight;

	const int gameW = Engine::Get().GetGameWidth();
	const int gameH = Engine::Get().GetGameHeight();

	const Vector2 gameCenter((gameW - 1) / 2, (gameH - 1) / 2);
	const Vector2 mapHalf(mapWidth / 2, mapHeight / 2);

	mapOrigin = gameCenter - mapHalf;

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

		// 월드좌표를 화면좌표로 변환해서 '그려질 위치'로 배치
		// (액터 position 자체를 화면좌표로 둘 거면 여기서 스크린좌표로 넣고,
		//  월드좌표를 유지하려면 Actor 쪽에 변환로직이 필요합니다.)
		// 현재 구조상 Actor가 position 그대로 Draw하므로, 여기서는 "스크린좌표"로 세팅합니다.
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

	if (!targetActor || targetActor->IsTypeOf<Wall>() || targetActor->IsTypeOf<Box>())
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
	std::vector<Actor*> enemies;

	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Enemy>())
			enemies.emplace_back(actor);
	}

	return enemies.size() == 0;
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
