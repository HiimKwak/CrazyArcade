#include <iostream>

#include "Game.h"
#include "Level/GameLevel.h"
#include "Level/MenuLevel.h"

Game* Game::instance = nullptr;

Game::Game()
{
	instance = this;

	levelList.emplace_back(new MenuLevel());
	levelList.emplace_back(new GameLevel());

	mainLevel = levelList[static_cast<int>(currentLevel)];
}

Game::~Game()
{
	mainLevel = nullptr;

	for (Level*& level : levelList)
	{
		delete level;
		level = nullptr;
	}

	levelList.clear();
}

void Game::ToggleMenu()
{
	system("cls");

	int levelIndex = static_cast<int>(currentLevel);
	int nextLevel = 1 - levelIndex;
	currentLevel = static_cast<State>(nextLevel);

	mainLevel = levelList[static_cast<int>(currentLevel)];
}

Game& Game::Get()
{
	if (!instance)
	{
		std::cerr << "Game:Get() - instance is null\n";
		__debugbreak();
	}

	return *instance;
}