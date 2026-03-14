#pragma once

#include "Engine/Engine.h"
#include "Render/Renderer.h"
#include "Math/Vector2.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/StatsComponent.h"
#include "Actor/Character/Component/MovementComponent.h"
#include "Actor/Character/Component/Item/ItemComponent.h"

#include <vector>
#include <string>
#include <cwchar>
#include <stdlib.h>

using namespace engine;

class Player;

class RenderService
{
public:
	virtual ~RenderService() = default;

	void SetCurrentStage(int stage)
	{
		currentStage = stage;
	}

	void SetDebugPathVisible(bool visible)
	{
		debugPathVisible = visible;
	}

	void SetActors(const std::vector<Actor*>* inActors)
	{
		actors = inActors;
	}

	void SetTileSize(int ts)
	{
		tileSize = ts;
	}

	void ShowShieldPopup()
	{
		popupVisible = true;
	}

	void HidePopup()
	{
		popupVisible = false;
	}

	bool IsPopupVisible() const
	{
		return popupVisible;
	}

	static int GetHudWidth()
	{
		return HUD_WIDTH;
	}

	void DrawHud(Player* player)
	{
		const int viewportW = engine::Engine::Get().GetWidth();
		const int viewportH = engine::Engine::Get().GetHeight();
		const int hudX = viewportW - HUD_WIDTH + 1;

		for (int y = 0; y < viewportH; ++y)
		{
			Renderer::Get().Submit(L"|", Vector2(hudX - 1, y), Color::White, 100);
		}

		swprintf_s(stageStr, _countof(stageStr), L"Stage: %d", currentStage);
		Renderer::Get().Submit(stageStr, Vector2(hudX, 1), Color::White, 100);

		if (player)
		{
			auto statsComp = player->GetComponent<StatsComponent>();
			auto movementComp = player->GetComponent<MovementComponent>();

			swprintf_s(hpStr, _countof(hpStr), L"HP: %d",
				statsComp ? statsComp->GetLives() : 0);
			Renderer::Get().Submit(hpStr, Vector2(hudX, 5), Color::Yellow, 100);

			swprintf_s(bubbleStr, _countof(bubbleStr), L"Bubble: %d/%d",
				statsComp ? statsComp->GetWaterBalloonAmmo() : 0,
				statsComp ? statsComp->GetMaxWaterBalloonAmmo() : 0);
			Renderer::Get().Submit(bubbleStr, Vector2(hudX, 7), Color::Yellow, 100);

			swprintf_s(rangeStr, _countof(rangeStr), L"Range: %d",
				statsComp ? statsComp->GetExplosionRange() : 0);
			Renderer::Get().Submit(rangeStr, Vector2(hudX, 9), Color::Yellow, 100);

			float displaySpeed = 1.0f;
			if (movementComp)
			{
				float currentSpeed = movementComp->GetMoveSpeed();
				if (currentSpeed > 0.0f)
					displaySpeed = MovementComponent::SPEED_NORMAL / currentSpeed;
			}
			swprintf_s(speedStr, _countof(speedStr), L"Speed: %.2fx", displaySpeed);
			Renderer::Get().Submit(speedStr, Vector2(hudX, 11), Color::Yellow, 100);

			Renderer::Get().Submit(L"Items:", Vector2(hudX, 14), Color::Skyblue, 100);

			auto itemComp = player->GetComponent<ItemComponent>();
			if (itemComp)
			{
				const auto& items = itemComp->GetAcquiredItems();
				int itemY = 15;

				if (items.empty())
				{
					Renderer::Get().Submit(L"  None", Vector2(hudX, itemY), Color::White, 100);
				}
				else
				{
					itemLineBuffers.clear();
					itemLineBuffers.reserve(items.size());
					for (const auto& [itemType, count] : items)
					{
						const wchar_t* itemName = GetItemName(itemType);
						wchar_t itemLine[80] = {};
						swprintf_s(itemLine, _countof(itemLine), L"%s x%d", itemName, count);
						itemLineBuffers.emplace_back(itemLine);
						Renderer::Get().Submit(itemLineBuffers.back().c_str(), Vector2(hudX, itemY++), Color::Green, 100);
					}
				}
			}
		}

		Renderer::Get().Submit(L"Menu: Esc", Vector2(hudX, viewportH - 3), Color::Orange, 100);
		Renderer::Get().Submit(L"Quit: q", Vector2(hudX, viewportH - 2), Color::Orange, 100);
	}

	void DrawDebugPathOverlay()
	{
		if (!debugPathVisible || !actors)
			return;

		for (Actor* actor : *actors)
		{
			if (!actor->IsTypeOf<Enemy>()) continue;
			const auto& path = actor->As<Enemy>()->GetDebugPath();
			for (int i = 1; i < static_cast<int>(path.size()); ++i)
			{
				int centerX = path[i].x + tileSize / 2;
				int centerY = path[i].y + tileSize / 2;
				Renderer::Get().SubmitPixel(centerX, centerY, Color::White, 4);
			}
		}
	}

	void DrawPopupOverlay()
	{
		if (popupVisible)
			DrawShieldPopup();
	}

	void DrawGameState(bool isGameOver, bool isGameClear, int remainingStages, float remainingTime)
	{
		const int centerX = (engine::Engine::Get().GetWidth() - HUD_WIDTH) / 2;
		const int centerY = engine::Engine::Get().GetHeight() / 2;

		if (isGameOver)
		{
			DrawPopupContainer(Color::Red);

			const wchar_t* title1 = L"█▀▀ ▄▀█ █▀▄▀█ █▀▀   █▀█ █░█ █▀▀ █▀█";
			const wchar_t* title2 = L"█▄█ █▀█ █░▀░█ ██▄   █▄█ ▀▄▀ ██▄ █▀▄";
			int title1Len = static_cast<int>(wcslen(title1));
			int title2Len = static_cast<int>(wcslen(title2));
			Renderer::Get().Submit(title1, Vector2(centerX - title1Len / 2, centerY - 4), Color::Red, 100);
			Renderer::Get().Submit(title2, Vector2(centerX - title2Len / 2, centerY - 3), Color::Red, 100);

			const wchar_t* msg1 = L"Press ESC to return to Main Menu";
			int msg1Len = static_cast<int>(wcslen(msg1));
			Renderer::Get().Submit(msg1, Vector2(centerX - msg1Len / 2, centerY + 2), Color::White, 100);
		}

		if (isGameClear)
		{
			DrawPopupContainer(Color::Yellow);

			const wchar_t* title1 = L"█▀ ▀█▀ ▄▀█ █▀▀ █▀▀   █▀▀ █░░ █▀▀ ▄▀█ █▀█";
			const wchar_t* title2 = L"▄█ ░█░ █▀█ █▄█ ██▄   █▄▄ █▄▄ ██▄ █▀█ █▀▄";
			int title1Len = static_cast<int>(wcslen(title1));
			int title2Len = static_cast<int>(wcslen(title2));
			Renderer::Get().Submit(title1, Vector2(centerX - title1Len / 2, centerY - 6), Color::Yellow, 100);
			Renderer::Get().Submit(title2, Vector2(centerX - title2Len / 2, centerY - 5), Color::Yellow, 100);

			wchar_t countdownStr[128];
			swprintf_s(countdownStr, _countof(countdownStr), L"Next stage in:  %d seconds", static_cast<int>(remainingTime) + 1);
			int countdownLen = static_cast<int>(wcslen(countdownStr));
			Renderer::Get().Submit(countdownStr, Vector2(centerX - countdownLen / 2, centerY - 1), Color::Skyblue, 100);

			if (remainingStages > 0)
			{
				const wchar_t* msg1 = L"Press N to skip to next stage";
				const wchar_t* msg2 = L"Press ESC to return to Main Menu";
				int msg1Len = static_cast<int>(wcslen(msg1));
				int msg2Len = static_cast<int>(wcslen(msg2));
				Renderer::Get().Submit(msg1, Vector2(centerX - msg1Len / 2, centerY + 3), Color::Green, 100);
				Renderer::Get().Submit(msg2, Vector2(centerX - msg2Len / 2, centerY + 5), Color::Orange, 100);
			}
			else
			{
				const wchar_t* msg1 = L"★ ALL STAGES CLEARED! ★";
				const wchar_t* msg2 = L"Press ESC to return to Main Menu";
				int msg1Len = static_cast<int>(wcslen(msg1));
				int msg2Len = static_cast<int>(wcslen(msg2));
				Renderer::Get().Submit(msg1, Vector2(centerX - msg1Len / 2, centerY + 3), Color::Yellow, 100);
				Renderer::Get().Submit(msg2, Vector2(centerX - msg2Len / 2, centerY + 5), Color::Orange, 100);
			}
		}
	}

private:
	void DrawPopupContainer(Color borderColor, int boxWidth = defaultPopupWidth, int boxHeight = defaultPopupHeight)
	{
		const int centerX = (engine::Engine::Get().GetWidth() - HUD_WIDTH) / 2;
		const int centerY = engine::Engine::Get().GetHeight() / 2;

		for (int y = centerY - boxHeight; y <= centerY + boxHeight; ++y)
		{
			for (int x = centerX - boxWidth; x <= centerX + boxWidth; ++x)
			{
				Renderer::Get().Submit(L" ", Vector2(x, y), Color::Black, 99);
			}
		}

		for (int x = centerX - boxWidth; x <= centerX + boxWidth; ++x)
		{
			Renderer::Get().Submit(L"═", Vector2(x, centerY - boxHeight), borderColor, 100);
			Renderer::Get().Submit(L"═", Vector2(x, centerY + boxHeight), borderColor, 100);
		}
		for (int y = centerY - boxHeight; y <= centerY + boxHeight; ++y)
		{
			Renderer::Get().Submit(L"║", Vector2(centerX - boxWidth, y), borderColor, 100);
			Renderer::Get().Submit(L"║", Vector2(centerX + boxWidth, y), borderColor, 100);
		}
		Renderer::Get().Submit(L"╔", Vector2(centerX - boxWidth, centerY - boxHeight), borderColor, 100);
		Renderer::Get().Submit(L"╗", Vector2(centerX + boxWidth, centerY - boxHeight), borderColor, 100);
		Renderer::Get().Submit(L"╚", Vector2(centerX - boxWidth, centerY + boxHeight), borderColor, 100);
		Renderer::Get().Submit(L"╝", Vector2(centerX + boxWidth, centerY + boxHeight), borderColor, 100);
	}

	void DrawShieldPopup()
	{
		const int centerX = (engine::Engine::Get().GetWidth() - HUD_WIDTH) / 2;
		const int centerY = engine::Engine::Get().GetHeight() / 2;

		DrawPopupContainer(Color::White, 30);

		const wchar_t* title1 = L"█▀ █▄█ █ █▀▀ █░░ █▀▄   ▄▀█ █▀▀ █▀█ █░█ █ █▀█ █▀▀ █▀▄";
		const wchar_t* title2 = L"▄█ █ █ █ ██▄ █▄▄ █▄▀   █▀█ █▄▄ ▀▀█ █▄█ █ █▀▄ ██▄ █▄▀";
		const wchar_t* msg1 = L"Press S to become invincible for 2 seconds.";

		int title1Len = static_cast<int>(wcslen(title1));
		int title2Len = static_cast<int>(wcslen(title2));
		int msg1Len = static_cast<int>(wcslen(msg1));
		Renderer::Get().Submit(title1, Vector2(centerX - title1Len / 2, centerY - 4), Color::White, 100);
		Renderer::Get().Submit(title2, Vector2(centerX - title2Len / 2, centerY - 3), Color::White, 100);
		Renderer::Get().Submit(msg1, Vector2(centerX - msg1Len / 2, centerY + 2), Color::White, 100);
	}

	int GetCenteredX(const wchar_t* text) const
	{
		const int viewportW = engine::Engine::Get().GetWidth();
		const int textLen = static_cast<int>(wcslen(text));
		return (viewportW - textLen) / 2;
	}

	const wchar_t* GetItemName(ItemType type) const
	{
		switch (type)
		{
		case ItemType::BubbleUpgrade:
			return L"  ◆ Bubble+";
		case ItemType::Roller:
			return L"  ◇ Roller";
		case ItemType::Shield:
			return L"  ◈ Shield";
		default:
			return L"  ? Unknown";
		}
	}

private:
	static constexpr int HUD_WIDTH = 20;
	static constexpr int defaultPopupWidth = 40;
	static constexpr int defaultPopupHeight = 16;

	bool popupVisible = false;

	int currentStage = 1;
	bool debugPathVisible = false;
	int tileSize = 32;
	const std::vector<Actor*>* actors = nullptr;

	wchar_t stageStr[64] = {};
	wchar_t hpStr[32] = {};
	wchar_t bubbleStr[32] = {};
	wchar_t rangeStr[32] = {};
	wchar_t speedStr[32] = {};
	std::vector<std::wstring> itemLineBuffers;
};
