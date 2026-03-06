#include "ShieldEffect.h"
#include "Actor/Actor.h"

void ShieldEffect::Apply(Actor* target)
{
	this->target = target;
	originalColor = target->GetSpriteColor();
	blinking = true;
	blinkTimer = 0.0f;
}

void ShieldEffect::Tick(float deltaTime)
{
	if (!blinking)
		return;

	blinkTimer += deltaTime;
	float elapsedTime = blinkTimer;
	int blinkCount = static_cast<int>(elapsedTime / blinkInterval);

	if (blinkCount % 2 == 0)
	{
		if (target)
			target->SetSpriteColor(Color::White);
	}
	else
	{
		if (target)
			target->SetSpriteColor(originalColor);
	}

	if (blinkTimer >= blinkDuration)
	{
		blinking = false;
		blinkTimer = 0.0f;
		if (target)
			target->SetSpriteColor(originalColor);
	}
}
