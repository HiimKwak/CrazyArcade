#pragma once

#include <vector>

#include "Engine/Engine.h"

using namespace engine;

enum class State
{
	None = -1,
	Menu = 0,
	GamePlay = 1,
	Length
};

class Game : public Engine
{
public:
	Game();
	~Game();

	void ToggleMenu();

	static Game& Get();

private:
	std::vector<Level*> levelList;

	State currentLevel = State::Menu;

	static Game* instance;
};

