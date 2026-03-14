#include <iostream>

#include "MenuLevel.h"
#include "GameLevel.h"
#include "Game/Game.h"
#include "Core/Input.h"
#include "Util/Util.h"
#include "Render/Renderer.h"

MenuLevel::MenuLevel()
{
	menuList.emplace_back(new MenuItem(
		L"New Game",
		[]() {
			Engine::Get().SetNewLevel(new GameLevel());
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
		Engine::Get().SetNewLevel(new GameLevel());
		currentIndex = 0;
	}
}

void MenuLevel::Draw()
{
	const wchar_t* title1 = L"   ⣠⣤⣤⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⡏⠙⣷⠀⠀     ";
	const wchar_t* title2 = L"⠀⣰⠟⠉⠀⠀⣈⣿⣤⠶⠶⣦⣀⣴⠶⠶⠶⣤⣀⡶⠶⠶⠶⠶⠶⡶⠶⣆⣠⡶⠶⣄⠀⠀⢠⡞⠋⠁⠉⠉⠛⣦⣀⡴⠶⠶⣆⣠⡴⠶⠶⣆⣠⡴⠶⠶⢶⣤⡀⢀⡤⠶⠶⢿⠃⠀⣿⣠⡴⠶⠶⢶⣄⠀";
	const wchar_t* title3 = L"⢸⡏⠀⣠⡞⠛⢻⡟⠁⢀⣠⣿⠏⠀⢀⣀⡀⠈⢻⣧⣤⡄⠀⠀⣰⠀⠀⣿⣿⠀⠀⣿⠀⠀⣿⠀⢀⡾⠻⣦⠀⢸⡏⠀⢀⣠⣿⠋⠀⣀⣤⣿⠋⠀⣀⣀⠀⠈⢿⡟⠁⢀⣠⣼⠀⠀⣿⠋⠀⣀⠀⠀⢸⡷";
	const wchar_t* title4 = L"⣿⡇⠀⣿⡀⠀⢸⡇⠀⢸⡏⣿⠀⠀⣿⣉⣿⠀⢸⣿⠟⠀⢀⣼⣿⠀⠀⣿⣿⠀⠀⣿⠀⠀⣿⠀⢸⡿⠟⠋⠀⢸⡇⠀⣿⠉⣿⠀⠸⣏⣉⣿⠀⠸⣏⣙⣷⠀⢸⡇⠀⢿⣉⣿⠃⠀⣿⠀⢸⡇⢀⣴⡟⠁";
	const wchar_t* title5 = L"⠸⣧⠀⠘⠷⢦⣼⡇⠀⣸⡇⠹⣦⡀⠈⠉⣿⠀⢸⠃⠀⠀⠉⠉⢻⣧⡀⠈⠉⠀⠀⣿⠀⠀⣿⠀⢸⣧⣤⣤⠀⢸⡇⠀⣿⠀⢻⣆⡀⠈⠙⣿⣆⠀⠉⠙⣿⠀⢸⣧⡀⠈⠉⠁⢀⣴⢿⣄⠀⠉⠉⠉⣿⠀";
	const wchar_t* title6 = L"⠀⠙⢷⣤⣀⣀⣨⡿⠶⠟⠀⠀⠈⠛⠷⠶⠛⠷⠾⠶⠶⠶⠶⠶⠟⠉⣻⡶⠖⠀⢀⣿⠀⠀⣿⡀⣸⡏⠈⣿⡀⣸⡷⠾⠋⠀⠀⠉⠛⠶⠾⠋⠙⠻⠶⠾⠛⠶⠟⠉⠙⠳⠶⠾⠛⠁⠀⠙⠻⠶⠶⠾⠛⠀";
	const wchar_t* title7 = L"   ⠈⠉⠙⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⣆⣀⣤⡾⠃⠀⠀⠈⠛⠋⠁⠀⠈⠛⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀";

	size_t title1_length = wcslen(title1);
	size_t title2_length = wcslen(title2);
	size_t title3_length = wcslen(title3);
	size_t title4_length = wcslen(title4);
	size_t title5_length = wcslen(title5);
	size_t title6_length = wcslen(title6);
	size_t title7_length = wcslen(title7);

	Vector2 title1_pos = GetCenterCoord(title1_length + 1) - Vector2(0, 10);
	Vector2 title2_pos = GetCenterCoord(title2_length + 1) - Vector2(0, 9);
	Vector2 title3_pos = GetCenterCoord(title3_length + 1) - Vector2(0, 8);
	Vector2 title4_pos = GetCenterCoord(title4_length + 1) - Vector2(0, 7);
	Vector2 title5_pos = GetCenterCoord(title5_length + 1) - Vector2(0, 6);
	Vector2 title6_pos = GetCenterCoord(title6_length + 1) - Vector2(0, 5);
	Vector2 title7_pos = GetCenterCoord(title7_length + 1) - Vector2(0, 4);

	Renderer::Get().Submit(title1, title1_pos, Color::Skyblue);
	Renderer::Get().Submit(title2, title2_pos, Color::Skyblue);
	Renderer::Get().Submit(title3, title3_pos, Color::Skyblue);
	Renderer::Get().Submit(title4, title4_pos, Color::Skyblue);
	Renderer::Get().Submit(title5, title5_pos, Color::Skyblue);
	Renderer::Get().Submit(title6, title6_pos, Color::Skyblue);
	Renderer::Get().Submit(title7, title7_pos, Color::Skyblue);

	for (int ix = 0; ix < static_cast<int>(menuList.size()); ++ix)
	{
		wchar_t* text = menuList[ix]->text;
		Color textColor =
			(ix == currentIndex) ? selectedColor : unselectedColor;
		size_t textLength = wcslen(text) + 1;
		Vector2 text_pos = GetCenterCoord(textLength) - Vector2(0, (-3 - ix));

		Renderer::Get().Submit(text, text_pos, textColor);
	}
}

