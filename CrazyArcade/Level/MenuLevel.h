#pragma once

#include <vector>

#include "Level/Level.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"

using namespace engine;

struct MenuItem {
	wchar_t* text = nullptr;

	using OnSelected = void (*)();

	MenuItem(const wchar_t* src, OnSelected onSelected) : onSelected(onSelected) {
		size_t length = wcslen(src) + 1;
		text = new wchar_t[length];
		wcscpy_s(text, length, src);
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

