#include "Game/Game.h"
#include "GameLevel.h"
#include "Core/Input.h"
#include "Util/Util.h"
#include "Actor/Effects/Bubble.h"
#include "Actor/Effects/ExplosionTile.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/State/StateComponent.h"
#include "Actor/Character/Component/Item/ItemComponent.h"
#include "Actor/Character/Component/StatsComponent.h"
#include "Actor/Environments/Ground.h"
#include "Actor/Environments/Wall.h"
#include "Actor/Environments/Bush.h"
#include "Actor/Environments/Box.h"
#include "Actor/Environments/Item.h"

#include "Engine/Engine.h"
#include "Math/Vector2.h"
#include "Render/Renderer.h"

#include <iostream>	
#include <Windows.h>
#include <string>
#include <limits>

using namespace engine;

Vector2 GameLevel::WorldToScreen(const Vector2& worldPos) const
{
	return mapOrigin + worldPos * Engine::Get().GetTileSize();
}

bool GameLevel::IsInsideGameMap_Screen(const Vector2& screenPos) const
{
	if (mapWidth <= 0 || mapHeight <= 0)
		return false;

	const int ts = Engine::Get().GetTileSize();
	const int left = mapOrigin.x;
	const int top = mapOrigin.y;
	const int right = mapOrigin.x + (mapWidth * ts - 1);
	const int bottom = mapOrigin.y + (mapHeight * ts - 1);

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

	const float blinkCycle = DEBUG_PATH_ON_TIME + DEBUG_PATH_OFF_TIME;
	debugPathBlinkElapsed += deltaTime;
	while (debugPathBlinkElapsed >= blinkCycle)
		debugPathBlinkElapsed -= blinkCycle;
	debugPathVisible = (debugPathBlinkElapsed < DEBUG_PATH_ON_TIME);

	// ���� ���� ó��
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

	// ���� Ŭ���� ó��
	if (CheckGameClear())
	{
		if (!isClearWaiting)
		{
			isClearWaiting = true;
			clearTimer.SetTargetTime(clearWaitTime);
			clearTimer.Reset();
		}

		clearTimer.Tick(deltaTime);

		// NŰ�� �����ų� 10�ʰ� ������ ���� ����������
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

		if (player && player->GetPosition() == explosionPos)
		{
			auto stateComp = player->GetComponent<StateComponent>();
			if (stateComp && stateComp->GetCurrentState() == StateType::Normal)
				stateComp->ChangeState(StateType::BubbleTrapped);
		}

		for (Enemy* enemy : enemies)
		{
			if (enemy->GetPosition() != explosionPos) continue;
			auto stateComp = enemy->GetComponent<StateComponent>();
			if (stateComp && stateComp->GetCurrentState() == StateType::Normal)
				stateComp->ChangeState(StateType::BubbleTrapped);
		}

		for (Box* box : boxes)
		{
			if (box->GetPosition() == explosionPos)
				box->OnDamaged();
		}
	}

	for (Enemy* enemy : enemies)
	{
		if (!player || enemy->GetPosition() != player->GetPosition())
			continue;

		auto playerState = player->GetComponent<StateComponent>();
		auto enemyState = enemy->GetComponent<StateComponent>();
		if (!playerState || !enemyState || playerState->IsDead() || enemyState->IsDead())
			continue;

		// 상대가 BubbleTrapped 상태일 때 접촉하면 처치
		if (playerState->IsBubbleTrapped())
			player->OnKilled();
		else if (enemyState->IsBubbleTrapped())
			enemy->OnDamaged();
	}
}

void GameLevel::Draw()
{
	super::Draw();

	// A* 경로 시각화 - 점멸 상태일 때만 흰색/낮은 우선순위로 표시
	if (debugPathVisible)
	{
		const int ts = Engine::Get().GetTileSize();
		for (Actor* actor : actors)
		{
			if (!actor->IsTypeOf<Enemy>()) continue;
			const auto& path = actor->As<Enemy>()->GetDebugPath();
			for (int i = 1; i < static_cast<int>(path.size()); ++i)
				Renderer::Get().SubmitRect(path[i].x, path[i].y, ts, ts, Color::White, 4);
		}
	}

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
		auto statsComp = player->GetComponent<StatsComponent>();
		swprintf_s(hpStr, 128, L"HP: %d", statsComp ? statsComp->GetLives() : 0);
		Renderer::Get().Submit(hpStr, Vector2(0, gameH + 1), Color::Red, 100);

		wchar_t bubbleStr[1024] = {};
		auto statsComp2 = player->GetComponent<StatsComponent>();
		swprintf_s(bubbleStr, 1024, L"Bubble: %d/%d  Range: %d",
			statsComp2 ? statsComp2->GetBubbleAmmo() : 0,
			statsComp2 ? statsComp2->GetMaxBubbleAmmo() : 0,
			statsComp2 ? statsComp2->GetBubbleRange() : 0);
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
	/** ���� �Ҷ����� **/
	char path[2048] = {};
	sprintf_s(path, 2048, "../Assets/%s", filename);

	FILE* file = nullptr;
	fopen_s(&file, path, "rb");

	if (!file)
	{
		std::cerr << "Failed to open map file.\n";
		__debugbreak();
	}

	/** ���ۿ� ��� **/
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	char* data = new char[fileSize + 1];
	size_t readSize = fread(data, sizeof(char), fileSize, file);
	data[fileSize] = '\0';

	// 1) �� ũ�� ����(mapWidth/mapHeight)
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

	// 가상 좌표계에서 맵 중앙 정렬
	const int ts = Engine::Get().GetTileSize();
	const int dts = Engine::Get().GetDisplayTileSize();
	const int virtualGameW = gameW * ts / dts;
	const int virtualGameH = gameH * 2 * ts / dts;
	const int mapPixelW = mapWidth * ts;
	const int mapPixelH = mapHeight * ts;

	mapOrigin = Vector2(
		((virtualGameW - mapPixelW) / 2 / ts) * ts,
		((virtualGameH - mapPixelH) / 2 / ts) * ts
	);
	screenCenter = Vector2(virtualGameW / 2, virtualGameH / 2);

	/** �о�� ���ڿ� �м�(parsing) **/
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
		{
			Player* player = new Player(screenPos);
			player->SetDelegate(this);
			AddNewActor(player);
			AddNewActor(new Ground(screenPos));
			break;
		}
		case '4':
		{
			Enemy* enemy = new Enemy(screenPos);
			enemy->SetDelegate(this);
			AddNewActor(enemy);
			AddNewActor(new Ground(screenPos));
			break;
		}
		case '5':
			AddNewActor(new Bush(screenPos));
			break;
		}

		++worldPos.x;
	}

	delete[] data;
	fclose(file);

	// �� �׵θ� ����
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
		if (actor->IsTypeOf<Player>() && actor->GetPosition() == itemPos)
		{
			Player* player = actor->As<Player>();
			auto itemComp = player->GetComponent<ItemComponent>();
			if (itemComp)
			{
				itemComp->OnItemAcquired(itemType);
			}
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
	return Vector2::Zero; // �÷��̾ ���� ���?
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
	if (stateComp && stateComp->GetCurrentState() == StateType::Normal && HasBubbleAt(targetPos))
	{
		if (!Push(currentPos, targetPos))
			return false;
	}

	return true;
}

bool GameLevel::OnRequestGenerateBubble(Character* character, const Vector2& position, int range)
{
	if (HasBubbleAt(position))
		return false;

	Bubble* bubble = new Bubble(position, range);
	bubble->SetOwnerCharacter(character);
	AddNewActor(bubble);
	return true;
}

bool GameLevel::OnQueryCanMove(const Vector2& from, const Vector2& to)
{
	return CanMove(from, to);
}

Vector2 GameLevel::OnQueryPlayerPosition()
{
	return GetPlayerPosition();
}

bool GameLevel::OnQueryHasBubbleAt(const Vector2& position)
{
	return HasBubbleAt(position);
}

bool GameLevel::OnQueryHasBoxAt(const Vector2& position)
{
	return HasBoxAt(position);
}

bool GameLevel::OnQueryIsExplosionDangerAt(const Vector2& position)
{
	if (HasExplosionAt(position))
		return true;

	for (Actor* actor : actors)
	{
		if (!actor->IsTypeOf<Bubble>()) continue;
		for (const Vector2& tile : actor->As<Bubble>()->GetPredictedDangerTiles())
		{
			if (tile == position) return true;
		}
	}
	return false;
}

float GameLevel::OnQueryExplosionTimeAt(const Vector2& position)
{
	if (HasExplosionAt(position))
		return 0.0f;

	float minTime = std::numeric_limits<float>::infinity();
	for (Actor* actor : actors)
	{
		if (!actor->IsTypeOf<Bubble>()) continue;
		Bubble* bubble = actor->As<Bubble>();
		for (const Vector2& tile : bubble->GetPredictedDangerTiles())
		{
			if (tile == position)
			{
				float t = bubble->GetRemainingTime();
				if (t < minTime) minTime = t;
				break;
			}
		}
	}
	return minTime;
}
