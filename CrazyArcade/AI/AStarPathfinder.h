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

// A* 기반 경로 탐색 유틸리티
// 모든 메서드는 정적(static)이며 상태를 갖지 않는다.
// 휴리스틱: 맨해튼 거리 (4방향 격자 이동에 대해 admissible & consistent)
class AStarPathfinder
{
public:
	using CanMoveFn = std::function<bool(const Vector2& from, const Vector2& to)>;
	using HasAtFn   = std::function<bool(const Vector2& pos)>;

	// 실제 이동 가능한 최단 경로의 다음 타일 위치를 반환한다.
	// 경로가 없거나 start == target 이면 Vector2::Zero 반환.
	static Vector2 GetNextStep(
		const Vector2& start,
		const Vector2& target,
		const CanMoveFn& canMove,
		int tileSize)
	{
		if (start == target) return Vector2::Zero;

		TileMap parent = RunAStar(start, target, canMove, tileSize);
		return BacktrackNextStep(start, target, parent, tileSize);
	}

	// start 부터 target 까지의 전체 경로(타일 위치 목록)를 반환한다.
	// 경로가 없으면 빈 벡터를 반환한다.
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

	// 박스를 투과하는 이상 경로(planning path) 상에서
	// 첫 번째 박스 바로 앞(= 물풍선을 놓을 타일)을 반환한다.
	// 이미 플레이어에게 도달 가능하거나 경로를 찾지 못하면 Vector2::Zero 반환.
	static Vector2 FindBoxPlacementTile(
		const Vector2& start,
		const Vector2& target,
		const CanMoveFn& canMoveNormal,
		const CanMoveFn& canMoveThroughBoxes,
		const HasAtFn& hasBox,
		int tileSize)
	{
		// 이미 실제 경로가 있으면 박스 제거 불필요
		{
			TileMap parent = RunAStar(start, target, canMoveNormal, tileSize);
			if (parent.count(ToKey(target, tileSize)) > 0)
				return Vector2::Zero;
		}

		// 박스 투과 A*로 이상 경로 탐색
		TileMap parent = RunAStar(start, target, canMoveThroughBoxes, tileSize);
		if (parent.count(ToKey(target, tileSize)) == 0)
			return Vector2::Zero;

		// 경로 재구성 (start → target 순서)
		std::vector<Vector2> path = BuildPath(start, target, parent, tileSize);

		// 경로상 첫 번째 박스 앞 타일 반환
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
		return { v.x / ts, v.y / ts };
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
		return std::abs(a.first - b.first) + std::abs(a.second - b.second);
	}

	static TileMap RunAStar(
		const Vector2& start,
		const Vector2& target,
		const CanMoveFn& canMove,
		int tileSize)
	{
		using Node = std::pair<int, TileKey>; // (f = g + h, key)
		std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;

		TileMap parent;
		std::map<TileKey, int> gScore;
		std::set<TileKey> closed;

		auto startKey  = ToKey(start, tileSize);
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
				Vector2 nextPos    = currentPos + Dirs()[i] * tileSize;
				auto    nextKey    = ToKey(nextPos, tileSize);

				if (closed.count(nextKey)) continue;
				if (!canMove(currentPos, nextPos)) continue;

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
		auto current  = targetKey;

		while (true)
		{
			auto prev = parent.at(current);
			if (prev == startKey)
				return FromKey(current, tileSize);
			current = prev;
		}
	}

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
