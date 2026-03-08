#include "Bubble.h"
#include "Level/Level.h"
#include "BubbleExplosion.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Component/BubbleComponent.h"
#include "Interface/IGameRuleManager.h"
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
	IGameRuleManager* gm = dynamic_cast<IGameRuleManager*>(owner);
	if (!gm) return;

	const int ts = Engine::Get().GetTileSize();
	const Vector2 origin = GetPosition();

	// 중심 타일은 항상 위험
	predictedDangerTiles.push_back(origin);

	// BubbleExplosion::PropagateInDirection 과 동일한 전파 로직
	static const Vector2 dirs[4] = {
		Vector2::Up, Vector2::Down, Vector2::Left, Vector2::Right
	};
	for (const Vector2& dir : dirs)
	{
		for (int i = 1; i <= range; ++i)
		{
			Vector2 tile = origin + dir * (i * ts);
			if (!gm->CanExplosionPenetrate(tile)) break;  // 벽 → 타일 포함하지 않고 중단
			predictedDangerTiles.push_back(tile);
			if (gm->HasBoxAt(tile)) break;                // 박스 → 타일 포함하고 중단
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
		owner->AddNewActor(new BubbleExplosion(GetPosition(), range));

	if (ownerCharacter)
	{
		auto bubbleComp = ownerCharacter->GetComponent<BubbleComponent>();
		if (bubbleComp)
			bubbleComp->OnBubbleExploded();
	}

	Destroy();
}
