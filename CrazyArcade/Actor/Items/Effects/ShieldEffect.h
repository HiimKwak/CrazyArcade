#pragma once

#include "IItemEffect.h"

class ShieldEffect : public IItemEffect
{
public:
	virtual void Apply(Character* target) override;
	virtual void Tick(float deltaTime) override;
	virtual bool IsActive() const override { return blinking; }

private:
	Character* target = nullptr;
	float blinkTimer = 0.0f;
	float blinkDuration = 2.0f;
	float blinkInterval = 0.2f;
	bool blinking = false;
};
