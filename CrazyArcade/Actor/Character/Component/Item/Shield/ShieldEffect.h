#pragma once

#include "../IEffect.h"
#include "Math/Color.h"

using namespace engine;

class ShieldEffect : public IEffect
{
public:
	virtual void Apply(Actor* target) override;
	virtual void Tick(float deltaTime) override;
	virtual bool IsActive() const override { return blinking; }

private:
	Actor* target = nullptr;
	Color originalColor;
	float blinkTimer = 0.0f;
	float blinkDuration = 2.0f;
	float blinkInterval = 0.2f;
	bool blinking = false;
};
