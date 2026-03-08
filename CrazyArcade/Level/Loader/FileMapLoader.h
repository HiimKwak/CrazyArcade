#pragma once

#include "MapLoader.h"
#include "Engine/Engine.h"

#include <iostream>
#include <cstdio>

class FileMapLoader : public MapLoader
{
public:
	void SetWorldToScreenFn(const std::function<engine::Vector2(const engine::Vector2&)>& fn)
	{
		worldToScreen = fn;
	}

	MapData LoadMap(const char* filename, const SpawnActorFn& onSpawnActor) override
	{
		MapData out;

		char path[2048] = {};
		sprintf_s(path, 2048, "../Assets/%s", filename);

		FILE* file = nullptr;
		fopen_s(&file, path, "rb");

		if (!file)
		{
			std::cerr << "Failed to open map file: " << filename << "\n";
			return out;
		}

		fseek(file, 0, SEEK_END);
		size_t fileSize = ftell(file);
		rewind(file);

		char* data = new char[fileSize + 1];
		size_t readSize = fread(data, sizeof(char), fileSize, file);
		data[fileSize] = '\0';

		// 맵 크기 계산
		int computedMapWidth = 0;
		int computedMapHeight = 0;
		int currentLineWidth = 0;

		for (size_t i = 0; i < fileSize; ++i)
		{
			if (data[i] == '\r')
				continue;

			if (data[i] == '\n')
			{
				if (currentLineWidth > 0)
				{
					if (currentLineWidth > computedMapWidth) computedMapWidth = currentLineWidth;
					computedMapHeight++;
					currentLineWidth = 0;
				}
				continue;
			}
			++currentLineWidth;
		}
		if (currentLineWidth > 0)
		{
			if (currentLineWidth > computedMapWidth) computedMapWidth = currentLineWidth;
			computedMapHeight++;
		}

		out.width = computedMapWidth;
		out.height = computedMapHeight;

		// 월드 좌표 계산
		const int gameW = engine::Engine::Get().GetGameWidth();
		const int gameH = engine::Engine::Get().GetGameHeight();
		const int ts = engine::Engine::Get().GetTileSize();
		const int dts = engine::Engine::Get().GetDisplayTileSize();
		const int virtualGameW = gameW * ts / dts;
		const int virtualGameH = gameH * 2 * ts / dts;
		const int mapPixelW = out.width * ts;
		const int mapPixelH = out.height * ts;

		out.origin = engine::Vector2(
			((virtualGameW - mapPixelW) / 2 / ts) * ts,
			((virtualGameH - mapPixelH) / 2 / ts) * ts
		);
		out.screenCenter = engine::Vector2(virtualGameW / 2, virtualGameH / 2);

		// 파일 파싱 및 액터 스폰
		int index = 0;
		engine::Vector2 worldPos = engine::Vector2::Zero;

		while (index < (int)fileSize)
		{
			char mapCharacter = data[index];
			++index;

			if (mapCharacter == '\r')
				continue;

			if (mapCharacter == '\n')
			{
				++worldPos.y;
				worldPos.x = 0;
				continue;
			}

			if (worldToScreen)
			{
				engine::Vector2 screenPos = worldToScreen(worldPos);
				onSpawnActor((int)mapCharacter, screenPos);
			}

			++worldPos.x;
		}

		delete[] data;
		fclose(file);

		return out;
	}

	void SpawnBoundaryWalls(const MapData& mapData, const SpawnActorFn& onSpawnActor)
	{
		if (!worldToScreen) return;

		const int ts = engine::Engine::Get().GetTileSize();

		// 위/아래 경계
		for (int x = -1; x <= mapData.width; ++x)
		{
			engine::Vector2 fenceUp = worldToScreen(engine::Vector2(x, -1));
			onSpawnActor('W', fenceUp);
			
			engine::Vector2 fenceDown = worldToScreen(engine::Vector2(x, mapData.height));
			onSpawnActor('W', fenceDown);
		}

		// 좌/우 경계
		for (int y = 0; y < mapData.height; ++y)
		{
			engine::Vector2 fenceLeft = worldToScreen(engine::Vector2(-1, y));
			onSpawnActor('W', fenceLeft);
			
			engine::Vector2 fenceRight = worldToScreen(engine::Vector2(mapData.width, y));
			onSpawnActor('W', fenceRight);
		}
	}

private:
	std::function<engine::Vector2(const engine::Vector2&)> worldToScreen;
};
