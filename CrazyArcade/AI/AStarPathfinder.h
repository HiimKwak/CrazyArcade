#pragma once

#include "Math/Vector2.h"
#include <functional>
#include <queue>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace engine;

class AStarPathfinder
{
public:
	using CanMoveFn = std::function<bool(const Vector2& from, const Vector2& to)>;
	using HasAtFn = std::function<bool(const Vector2& pos)>;

	// Get next tile from shortest Astar path
	static Vector2 GetNextStep(
		const Vector2& start,
		const Vector2& target,
		const CanMoveFn& canMove,
		int tileSize)
	{
		if (start == target) return Vector2::Zero;

		TileMap parent = RunAStar(start, target, canMove, tileSize); // get astar path map
		return BacktrackNextStep(start, target, parent, tileSize); // get next tile from astar path map
	}

	// returns a shortest Astar path 2D vector list
	static std::vector<Vector2> GetFullPath(
		const Vector2& start,
		const Vector2& target,
		const CanMoveFn& canMove,
		int tileSize)
	{
		if (start == target) return {};

		TileMap parent = RunAStar(start, target, canMove, tileSize);
		return BuildPath(start, target, parent, tileSize);
	}

	static Vector2 FindBoxPlacementTile(
		const Vector2& start,
		const Vector2& target,
		const CanMoveFn& canMoveNormal,
		const CanMoveFn& canMoveThroughBoxes,
		const HasAtFn& hasBox,
		int tileSize)
	{
		{
			// exit the process if character can move towards target without destroying boxes
			TileMap parent = RunAStar(start, target, canMoveNormal, tileSize);
			if (parent.count(ToKey(target, tileSize)) > 0)
				return Vector2::Zero;
		}

		// find shortest destructible path
		TileMap parent = RunAStar(start, target, canMoveThroughBoxes, tileSize);
		if (parent.count(ToKey(target, tileSize)) == 0) // no destructible tiles around
			return Vector2::Zero;

		// set new path towards target
		std::vector<Vector2> path = BuildPath(start, target, parent, tileSize);

		// return the first box on the valid path
		for (int i = 1; i < static_cast<int>(path.size()); ++i)
		{
			if (hasBox(path[i]))
				return path[i - 1];
		}

		return Vector2::Zero;
	}

private:
	using TileKey = std::pair<int, int>;
	using TileMap = std::map<TileKey, TileKey>;

	static TileKey ToKey(const Vector2& v, int ts)
	{
		return { v.x / ts, v.y / ts }; // returns virtual world coords
	}

	static Vector2 FromKey(const TileKey& k, int ts)
	{
		return Vector2(k.first * ts, k.second * ts);
	}

	static const Vector2* Dirs()
	{
		static const Vector2 dirs[4] = {
			Vector2::Right, Vector2::Left, Vector2::Up, Vector2::Down
		};
		return dirs;
	}

	static int Heuristic(const TileKey& a, const TileKey& b)
	{
		return std::abs(a.first - b.first) + std::abs(a.second - b.second); // manhattan distance
	}

	static TileMap RunAStar(
		const Vector2& start,
		const Vector2& target,
		const CanMoveFn& canMove,
		int tileSize)
	{
		using Node = std::pair<int, TileKey>; // (cost(f = g + h), key)
		std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;

		TileMap parent;
		std::map<TileKey, int> gScore;
		std::set<TileKey> closed;

		auto startKey = ToKey(start, tileSize);
		auto targetKey = ToKey(target, tileSize);

		gScore[startKey] = 0;
		parent[startKey] = startKey;
		openSet.push({ Heuristic(startKey, targetKey), startKey });

		const int MAX_ITER = 2048;
		int iter = 0;

		while (!openSet.empty() && iter++ < MAX_ITER)
		{
			auto [f, current] = openSet.top();
			openSet.pop();

			if (closed.count(current)) continue;
			closed.insert(current);

			if (current == targetKey) break;

			for (int i = 0; i < 4; ++i)
			{
				Vector2 currentPos = FromKey(current, tileSize);
				Vector2 nextPos = currentPos + Dirs()[i] * tileSize;
				auto    nextKey = ToKey(nextPos, tileSize);

				// filter visited or blocked tiles
				if (closed.count(nextKey)) continue;
				if (!canMove(currentPos, nextPos)) continue;

				// set cheapest tile around character and put in openSet
				int newG = gScore[current] + 1;
				auto it = gScore.find(nextKey);
				if (it == gScore.end() || newG < it->second)
				{
					gScore[nextKey] = newG;
					parent[nextKey] = current;
					openSet.push({ newG + Heuristic(nextKey, targetKey), nextKey });
				}
			}
		}

		return parent;
	}

	static Vector2 BacktrackNextStep(
		const Vector2& start,
		const Vector2& target,
		const TileMap& parent,
		int tileSize)
	{
		auto targetKey = ToKey(target, tileSize);
		if (parent.count(targetKey) == 0)
			return Vector2::Zero;

		auto startKey = ToKey(start, tileSize);
		auto current = targetKey;

		while (true)
		{
			auto prev = parent.at(current);
			if (prev == startKey)
				return FromKey(current, tileSize);
			current = prev;
		}
	}

	// converts a tilemap to a 2D vector list
	static std::vector<Vector2> BuildPath(
		const Vector2& start,
		const Vector2& target,
		const TileMap& parent,
		int tileSize)
	{
		auto targetKey = ToKey(target, tileSize);
		if (parent.count(targetKey) == 0) return {};

		auto startKey = ToKey(start, tileSize);
		std::vector<Vector2> path;

		auto current = targetKey;
		while (current != startKey)
		{
			path.push_back(FromKey(current, tileSize));
			current = parent.at(current);
		}
		path.push_back(start);
		std::reverse(path.begin(), path.end());
		return path;
	}
};
