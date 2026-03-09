#include "Bubble.h"
#include "Level/Level.h"
#include "Actor/Effects/ExplosionTile.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Component/BubbleComponent.h"
#include "Interface/IWorldQueryProvider.h"
#include "Interface/IWorldQueryService.h"
#include "Engine/Engine.h"

Bubble::Bubble(const Vector2& newPosition, int explosionRange)
	: super(L"B", newPosition, Color::Blue),
	range(explosionRange)
{
	sortingOrder = 5;
	explodeTimer.SetTargetTime(countDown);
	explodeTimer.Reset();
}

void Bubble::BeginPlay()
{
	super::BeginPlay();
	ComputeDangerTiles();
}

void Bubble::ComputeDangerTiles()
{
	IWorldQueryProvider* provider = dynamic_cast<IWorldQueryProvider*>(owner);
	IWorldQueryService* worldQuery = provider ? provider->GetWorldQueryService() : nullptr;
	if (!worldQuery) return;

	const int ts = Engine::Get().GetTileSize();
	const Vector2 origin = GetPosition();

	predictedDangerTiles.clear();
	predictedDangerTiles.push_back(origin);

	static const Vector2 dirs[4] = {
		Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
	};
	for (const Vector2& dir : dirs)
	{
		for (int i = 1; i <= range; ++i)
		{
			Vector2 tile = origin + dir * (i * ts);
			if (!worldQuery->CanExplosionPenetrate(tile)) break;
			predictedDangerTiles.push_back(tile);
			if (worldQuery->HasBoxAt(tile)) break;
		}
	}
}

void Bubble::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	Explode(deltaTime);
}

bool Bubble::IsExploded() const
{
	return exploded;
}

void Bubble::Explode(float deltaTime)
{
	explodeTimer.Tick(deltaTime);

	if (!explodeTimer.IsTimeout())
	{
		float remainingTime = explodeTimer.GetRemainingTime();
		int blink = 0;
		if (remainingTime <= 1.0f)
			blink = static_cast<int>(remainingTime * 10) % 2;
		else if (remainingTime <= 2.0f)
			blink = static_cast<int>(remainingTime * 5) % 2;
		else if (remainingTime <= 3.0f)
			blink = static_cast<int>(remainingTime * 3.33f) % 2;

		return this->sprite.SetColor(blink == 0 ? Color::Blue : Color::Skyblue);
	}

	explodeTimer.Reset();
	exploded = true;

	if (owner)
	{
		IWorldQueryProvider* provider = dynamic_cast<IWorldQueryProvider*>(owner);
		IWorldQueryService* worldQuery = provider ? provider->GetWorldQueryService() : nullptr;
		const int ts = Engine::Get().GetTileSize();
		const Vector2 origin = GetPosition();

		// 중심 타일
		ExplosionTile* center = new ExplosionTile(origin);
		center->Activate();
		owner->AddNewActor(center);

		// 4방향 전파 (BubbleExplosion::PropagateInDirection과 동일한 차단 규칙)
		static const Vector2 dirs[4] = {
			Vector2::Up, Vector2::Right, Vector2::Down, Vector2::Left
		};
		for (const Vector2& dir : dirs)
		{
			for (int i = 1; i <= range; ++i)
			{
				Vector2 tilePos = origin + (i * ts) * dir;

				if (worldQuery && !worldQuery->CanExplosionPenetrate(tilePos))
					break;

				ExplosionTile* tile = new ExplosionTile(tilePos);
				tile->Activate();
				owner->AddNewActor(tile);

				if (worldQuery && worldQuery->HasBoxAt(tilePos))
					break;
			}
		}
	}

	if (ownerCharacter)
	{
		auto bubbleComp = ownerCharacter->GetComponent<BubbleComponent>();
		if (bubbleComp)
			bubbleComp->OnBubbleExploded();
	}

	Destroy();
}
