#include "BubbleComponent.h"
#include "StatsComponent.h"
#include "../Character.h"

void BubbleComponent::Tick(float deltaTime)
{
}

bool BubbleComponent::RequestGenerateBubble()
{
	if (!owner)
		return false;

	auto stats = owner->GetComponent<StatsComponent>();
	if (!stats || stats->GetBubbleAmmo() <= 0)
		return false;

	auto delegate = owner->GetDelegate();
	if (!delegate)
		return false;

	if (delegate->OnRequestGenerateBubble(owner, owner->GetPosition(), stats->GetBubbleRange()))
	{
		stats->SetBubbleAmmo(stats->GetBubbleAmmo() - 1);
		return true;
	}
	return false;
}

void BubbleComponent::OnBubbleExploded()
{
	if (!owner)
		return;

	auto stats = owner->GetComponent<StatsComponent>();
	if (stats)
		stats->RestoreBubbleAmmo();
}
