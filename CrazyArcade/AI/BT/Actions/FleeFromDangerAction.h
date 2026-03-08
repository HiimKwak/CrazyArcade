#pragma once

#include "AI/BTNode.h"
#include "Math/Vector2.h"
#include "Engine/Engine.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Character/Component/MovementComponent.h"
#include "Actor/MoveSpeed.h"
#include <queue>
#include <map>
#include <utility>
#include <limits>

using namespace engine;

// BFS로 예상 폭발 범위 바깥의 안전 지대까지 최단 경로를 계산해 한 칸 이동한다.
//
// 탈출 조건:
//   - 이동할 타일이 폭발 범위 바깥이면 즉시 목적지로 확정
//   - 이동할 타일이 폭발 범위 안이더라도 도착 예상 시간 < 폭발 잔여 시간이면 경유 허용
//   - QueryCanMove가 물풍선 push를 포함하므로 push 경로도 자동 탐색
class FleeFromDangerAction : public engine::BTNode
{
public:
	FleeFromDangerAction(Enemy* enemy, MovementComponent* movement)
		: enemy(enemy), movement(movement)
	{
	}

	engine::BTStatus Execute(float) override
	{
		if (!enemy || !movement) return engine::BTStatus::Failure;
		if (movement->IsMoving())  return engine::BTStatus::Running;

		const Vector2 pos = enemy->GetPosition();
		const int ts = Engine::Get().GetTileSize();

		Vector2 dir = FindEscapeDir(pos, ts);
		if (dir == Vector2::Zero)
			return engine::BTStatus::Failure;

		movement->RequestMove(dir);
		return engine::BTStatus::Success;
	}

private:
	static constexpr int MAX_DEPTH = 8;

	using TileKey = std::pair<int, int>;

	TileKey Key(const Vector2& v, int ts) const
	{
		return { v.x / ts, v.y / ts };
	}

	// BFS: 가장 가까운 안전 타일(폭발 범위 바깥)로 가기 위한 첫 이동 방향 반환
	// 위험 타일은 (도착 예상 시간 < 폭발 잔여 시간)일 때만 경유 허용
	// 안전 지대가 없으면 Vector2::Zero 반환
	Vector2 FindEscapeDir(const Vector2& start, int ts) const
	{
		static const Vector2 dirs[4] = {
			Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
		};

		std::queue<Vector2> q;
		std::map<TileKey, Vector2> firstStep; // 각 타일까지의 start 기준 첫 방향
		std::map<TileKey, int> depth;         // 해당 타일까지의 이동 횟수

		firstStep[Key(start, ts)] = Vector2::Zero;
		depth[Key(start, ts)] = 0;
		q.push(start);

		while (!q.empty())
		{
			Vector2 cur = q.front();
			q.pop();

			int d = depth[Key(cur, ts)];
			if (d >= MAX_DEPTH) continue;

			for (const Vector2& dir : dirs)
			{
				Vector2 next = cur + dir * ts;
				TileKey nk   = Key(next, ts);

				if (firstStep.count(nk))           
					continue; // 이미 방문
				if (!enemy->QueryCanMove(cur, next)) 
					continue;

				// start에서 처음 취한 방향 기록
				Vector2 fd = (Key(cur, ts) == Key(start, ts))
					? dir
					: firstStep[Key(cur, ts)];

				firstStep[nk] = fd;
				depth[nk]     = d + 1;

				if (!enemy->QueryIsExplosionDangerAt(next))
					return fd;
				
				// 위험 타일: 도착 예상 시간 < 폭발 잔여 시간이면 경유 허용
				// todo: 경유 허용 지역이어도 이득이 아니면 경유하지 않게 설정
				float arrivalTime   = (d + 1) * MoveSpeed::NORMAL;
				float explosionTime = enemy->QueryExplosionTimeAt(next);
				if (arrivalTime < explosionTime)
					q.push(next);
			}
		}

		return Vector2::Zero;
	}

	Enemy* enemy;
	MovementComponent* movement;
};
