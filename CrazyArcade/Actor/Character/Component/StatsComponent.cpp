#include "StatsComponent.h"

void StatsComponent::Tick(float deltaTime)
{
}

void StatsComponent::RestoreBubbleAmmo()
{
	if (bubbleAmmo < maxBubbleAmmo)
		bubbleAmmo++;
}
