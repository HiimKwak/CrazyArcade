#include "StatsComponent.h"

void StatsComponent::Tick(float deltaTime)
{
}

void StatsComponent::RestoreWaterBalloonAmmo()
{
	if (waterBalloonAmmo < maxWaterBalloonAmmo)
		waterBalloonAmmo++;
}
