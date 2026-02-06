#pragma once

#include <vector>

#include "Level/Level.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"

using namespace engine;

struct MenuItem {
	char* text = nullptr;

	using OnSelected = void (*)();

	MenuItem(const char* text, OnSelected onSelected) : onSelected(onSelected) {
		size_t length = strlen(text) + 1;
		this->text = new char[length];
		strcpy_s(this->text, length, text);
	}

	~MenuItem()
	{
		if (text)
		{
			delete[] text;
			text = nullptr;
		}
	}

	OnSelected onSelected = nullptr;
};

class MenuLevel : public Level
{
	RTTI_DECLARATIONS(MenuLevel, Level)

public:
	MenuLevel();
	~MenuLevel();

	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	inline Vector2 GetCenterCoord(int textLength)
	{
		int screenWidth = Engine::Get().GetWidth();
		int screenHeight = Engine::Get().GetHeight();
		Vector2 v((screenWidth - textLength) / 2, screenHeight / 2);
		return v;
	};

private:
	int currentIndex = 0;

	Color selectedColor = Color::Green;
	Color unselectedColor = Color::White;

	std::vector<MenuItem*> menuList;
};

