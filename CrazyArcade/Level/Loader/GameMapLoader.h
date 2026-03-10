#pragma once

#include "MapLoader.h"
#include "Engine/Engine.h"

class GameMapLoader : public MapLoader
{
protected:
	void CalculateMapOrigin(MapData& mapData) override
	{
		const int ts = engine::Engine::Get().GetTileSize();
		const int dts = engine::Engine::Get().GetDisplayTileSize();
		const int viewportW = engine::Engine::Get().GetWidth();
		const int viewportH = engine::Engine::Get().GetHeight();

		const int gameAreaW = viewportW - hudWidth;
		const int gameAreaH = viewportH;

		const int virtualGameAreaW = gameAreaW * ts / dts;
		const int virtualGameAreaH = gameAreaH * 2 * ts / dts;
		const int mapPixelW = mapData.width * ts;
		const int mapPixelH = mapData.height * ts;

		mapData.origin = engine::Vector2(
			((virtualGameAreaW - mapPixelW) / 2 / ts) * ts,
			((virtualGameAreaH - mapPixelH) / 2 / ts) * ts
		);
		mapData.screenCenter = engine::Vector2(virtualGameAreaW / 2, virtualGameAreaH / 2);
	}
};
