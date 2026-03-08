#pragma once

#include "Math/Vector2.h"
#include <functional>

class Level;

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

	virtual MapData LoadMap(const char* filename, const SpawnActorFn& onSpawnActor) = 0;
};
