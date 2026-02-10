#include <iostream>

#include "MenuLevel.h"
#include "Game/Game.h"
#include "Core/Input.h"
#include "Util/Util.h"
#include "Render/Renderer.h"

MenuLevel::MenuLevel()
{
	menuList.emplace_back(new MenuItem(
		L"Resume Game",
		[]() {
			Game::Get().ToggleMenu();
		}
	));
	menuList.emplace_back(new MenuItem(
		L"Quit Game",
		[]() {
			Game::Get().Quit();
		}
	)
	);
}

MenuLevel::~MenuLevel()
{
	for (MenuItem*& item : menuList)
	{
		delete item;
		item = nullptr;
	}
	menuList.clear();
}

void MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	static int length = static_cast<int>(menuList.size());
	if (Input::Get().GetKeyDown(VK_UP))
	{
		currentIndex = (currentIndex - 1 + length) % length;
	}
	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		currentIndex = (currentIndex + 1) % length;
	}
	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		menuList[currentIndex]->onSelected();
	}
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		Game::Get().ToggleMenu();

		currentIndex = 0;
	}
}

void MenuLevel::Draw()
{
	const wchar_t* title = L"Crazy Arcade";
	size_t title_length = wcslen(title) + 1;
	Vector2 title_pos = GetCenterCoord(title_length) - Vector2(0, 5);
	Renderer::Get().Submit(L"Crazy Arcade", title_pos);

	for (int ix = 0; ix < static_cast<int>(menuList.size()); ++ix)
	{
		wchar_t* text = menuList[ix]->text;
		Color textColor =
			(ix == currentIndex) ? selectedColor : unselectedColor;
		size_t textLength = wcslen(text) + 1;
		Vector2 text_pos = GetCenterCoord(textLength) - Vector2(0, (3 - ix));

		Renderer::Get().Submit(text, text_pos, textColor);
	}
}

