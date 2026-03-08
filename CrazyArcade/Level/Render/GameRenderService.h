#pragma once

#include "RenderService.h"
#include "Engine/Engine.h"
#include "Render/Renderer.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/StatsComponent.h"

#include <vector>

using namespace engine;

class GameRenderService : public RenderService
{
public:
	void SetCurrentStage(int stage) override
	{
		currentStage = stage;
	}

	void SetDebugPathVisible(bool visible) override
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

	void DrawHud(Player* player) override
	{
		const int gameH = engine::Engine::Get().GetGameHeight();

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
	}

	void DrawDebugPathOverlay() override
	{
		if (!debugPathVisible || !actors)
			return;

		for (Actor* actor : *actors)
		{
			if (!actor->IsTypeOf<Enemy>()) continue;
			const auto& path = actor->As<Enemy>()->GetDebugPath();
			for (int i = 1; i < static_cast<int>(path.size()); ++i)
				Renderer::Get().SubmitRect(path[i].x, path[i].y, tileSize, tileSize, Color::White, 4);
		}
	}

	void DrawGameState(bool isGameOver, bool isGameClear) override
	{
		const int gameH = engine::Engine::Get().GetGameHeight();

		if (isGameOver)
			Renderer::Get().Submit(L"Game Over!", Vector2(0, gameH + 3), Color::Red, 100);

		if (isGameClear)
			Renderer::Get().Submit(L"Game Clear!", Vector2(0, gameH + 3), Color::White, 100);
	}

private:
	int currentStage = 1;
	bool debugPathVisible = false;
	int tileSize = 32;
	const std::vector<Actor*>* actors = nullptr;
};
