#pragma once

#include "Math/Vector2.h"

using namespace engine;

class Player;

class RenderService
{
public:
	virtual ~RenderService() = default;

	virtual void SetCurrentStage(int stage) = 0;
	virtual void SetDebugPathVisible(bool visible) = 0;

	virtual void DrawHud(Player* player) = 0;
	virtual void DrawDebugPathOverlay() = 0;
	virtual void DrawGameState(bool isGameOver, bool isGameClear) = 0;
};
