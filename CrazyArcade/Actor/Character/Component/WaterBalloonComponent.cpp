#include "WaterBalloonComponent.h"
#include "StatsComponent.h"
#include "../Character.h"
#include "Engine/Engine.h"

using namespace engine;

void WaterBalloonComponent::Tick(float deltaTime)
{
}

bool WaterBalloonComponent::RequestGenerateWaterBalloon()
{
	if (!owner)
		return false;

	auto stats = owner->GetComponent<StatsComponent>();
	if (!stats || stats->GetWaterBalloonAmmo() <= 0)
		return false;

	auto delegate = GetCharacter()->GetDelegate();
	if (!delegate)
		return false;

	// 타일 기준으로 위치를 스냅
	const int ts = Engine::Get().GetTileSize();
	Vector2 currentPos = owner->GetPosition();
	Vector2 tilePos = Vector2(
		(currentPos.x / ts) * ts,
		(currentPos.y / ts) * ts
	);

	if (delegate->OnRequestGenerateWaterBalloon(GetCharacter(), tilePos, stats->GetExplosionRange()))
	{
		stats->SetWaterBalloonAmmo(stats->GetWaterBalloonAmmo() - 1);
		return true;
	}
	return false;
}

void WaterBalloonComponent::OnWaterBalloonExploded()
{
	if (!owner)
		return;

	auto stats = owner->GetComponent<StatsComponent>();
	if (stats)
		stats->RestoreWaterBalloonAmmo();
}
