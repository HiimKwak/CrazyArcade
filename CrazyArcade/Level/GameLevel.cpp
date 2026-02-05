#include "GameLevel.h"
#include "Util/Util.h"
#include "Actor/Bubble.h"
#include "Actor/Ground.h"
#include "Actor/Player.h"
#include "Actor/Enemy.h"
#include "Actor/Wall.h"

#include <iostream>

GameLevel::GameLevel()
{
	LoadMap("Stage1.txt");
}
void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	ProcessExplosionCollision();
}

void GameLevel::ProcessExplosionCollision()
{
	std::vector<Bubble*> bubbles;
	Player* player;
	std::vector<Enemy*> enemies;

	for (Actor* actor : actors)
	{
		if (actor->IsTypeOf<Bubble>())
			bubbles.push_back(actor->As<Bubble>());

		if (actor->IsTypeOf<Player>())
			player = actor->As<Player>();

		if (actor->IsTypeOf<Enemy>())
			enemies.push_back(actor->As<Enemy>());
	}

	for (Bubble* bubble : bubbles)
	{
		if (!bubble->IsExploded()) continue;

		if (bubble->IsInExplosionRange(player))
			player->OnDamaged();

		for (Enemy* enemy : enemies)
		{
			if (bubble->IsInExplosionRange(enemy))
				enemy->OnDamaged();
		}
	}
}

void GameLevel::Draw()
{
	super::Draw();

	if (isGameClear)
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

	/** 읽어온 문자열 분석(parsing)해서 출력 **/
	int index = 0;
	engine::Vector2 position;

	while (index < fileSize) // todo: 조건 수정했음 안돌아가면 이거 봐야함
	{
		char mapCharacter = data[index];
		++index;

		if (mapCharacter == '\n')
		{
			++position.y;
			position.x = 0;
			continue;
		}

		switch (mapCharacter)
		{
		case '1':
			AddNewActor(new Wall(position));
			break;
		case '.':
			AddNewActor(new Ground(position));
			break;
		case 'p':
			AddNewActor(new Player(position));
			AddNewActor(new Ground(position));
			break;
		case 'e':
			AddNewActor(new Enemy(position));
			AddNewActor(new Ground(position));
			break;
		}

		++position.x;
	}

	delete[] data;

	fclose(file);
}

bool GameLevel::CanMove(const engine::Vector2& playerPosition, const engine::Vector2& nextPosition)
{
	std::vector<Actor*> bubbles;
	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Bubble>())
		{
			bubbles.emplace_back(actor);
			continue;
		}
	}

	Actor* bubbleActor = nullptr;
	for (Actor* const bubble : bubbles)
	{
		if (bubble->GetPosition() == nextPosition)
		{
			bubbleActor = bubble;
			break;
		}
	}

	if (bubbleActor)
	{
		Vector2 direction = nextPosition - playerPosition;
		Vector2 newPosition = bubbleActor->GetPosition() + direction;

		for (Actor* const actor : actors)
		{
			if (newPosition == actor->GetPosition())
			{
				if (actor->IsTypeOf<Wall>() || actor->IsTypeOf<Bubble>() || actor->IsTypeOf<Player>())
				{
					return false;
				}
				if (actor->IsTypeOf<Ground>() || actor->IsTypeOf<Enemy>()) // todo: bush?
				{
					bubbleActor->SetPosition(newPosition);

					return true;
				}
			}
		}
	}
	else {
		for (Actor* const actor : actors)
		{
			if (nextPosition == actor->GetPosition())
			{
				return actor->IsTypeOf<Wall>() ? false : true;
			}
		}
	}

	return false;
}

bool GameLevel::HasBubbleAt(const Vector2& position)
{
	for (Actor* actor : actors)
	{
		if (actor->GetPosition() == position)
			return true;
	}
	return false;
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
