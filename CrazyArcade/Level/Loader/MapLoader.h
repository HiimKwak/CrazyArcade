#pragma once

#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct MapData
{
	int width = 0;
	int height = 0;
	engine::Vector2 origin = engine::Vector2::Zero;
	engine::Vector2 screenCenter = engine::Vector2::Zero;
};

using SpawnActorFn = std::function<void(int /*character*/, const engine::Vector2& /*position*/)>;

class MapLoader
{
public:
	virtual ~MapLoader() = default;

	void SetHudWidth(int width)
	{
		hudWidth = width;
	}

	MapData LoadMap(const char* filename, const SpawnActorFn& onSpawnActor)
	{
		MapData out;

		std::string path = std::string("../Assets/") + filename;
		
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			std::cerr << "Failed to open map file: " << path << "\n";
			return out;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string data = buffer.str();

		int computedMapWidth = 0;
		int computedMapHeight = 0;
		int currentLineWidth = 0;

		for (char ch : data)
		{
			if (ch == '\r')
				continue;

			if (ch == '\n')
			{
				if (currentLineWidth > 0)
				{
					if (currentLineWidth > computedMapWidth)
						computedMapWidth = currentLineWidth;
					computedMapHeight++;
					currentLineWidth = 0;
				}
				continue;
			}
			++currentLineWidth;
		}
		
		if (currentLineWidth > 0)
		{
			if (currentLineWidth > computedMapWidth)
				computedMapWidth = currentLineWidth;
			computedMapHeight++;
		}

		out.width = computedMapWidth;
		out.height = computedMapHeight;

		CalculateMapOrigin(out);

		const int ts = engine::Engine::Get().GetTileSize();
		engine::Vector2 worldPos = engine::Vector2::Zero;

		for (char mapCharacter : data)
		{
			if (mapCharacter == '\r')
				continue;

			if (mapCharacter == '\n')
			{
				++worldPos.y;
				worldPos.x = 0;
				continue;
			}

			engine::Vector2 screenPos = out.origin + worldPos * ts;
			onSpawnActor((int)mapCharacter, screenPos);

			++worldPos.x;
		}

		return out;
	}

	void SpawnBoundaryWalls(const MapData& mapData, const SpawnActorFn& onSpawnActor)
	{
		const int ts = engine::Engine::Get().GetTileSize();

		for (int x = -1; x <= mapData.width; ++x)
		{
			engine::Vector2 fenceUp = mapData.origin + engine::Vector2(x, -1) * ts;
			onSpawnActor('W', fenceUp);
			
			engine::Vector2 fenceDown = mapData.origin + engine::Vector2(x, mapData.height) * ts;
			onSpawnActor('W', fenceDown);
		}

		for (int y = 0; y < mapData.height; ++y)
		{
			engine::Vector2 fenceLeft = mapData.origin + engine::Vector2(-1, y) * ts;
			onSpawnActor('W', fenceLeft);
			
			engine::Vector2 fenceRight = mapData.origin + engine::Vector2(mapData.width, y) * ts;
			onSpawnActor('W', fenceRight);
		}
	}

protected:
	virtual void CalculateMapOrigin(MapData& mapData) = 0;

protected:
	int hudWidth = 0;
};
