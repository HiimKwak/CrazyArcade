#pragma once

#include "ICharacterComponent.h"

class StatsComponent : public ICharacterComponent
{
public:
	virtual void Tick(float deltaTime) override;

	int GetLives() const { return lives; }
	void SetLives(int newLives) { lives = newLives; }

	int GetMaxBubbleAmmo() const { return maxBubbleAmmo; }
	int GetBubbleAmmo() const { return bubbleAmmo; }
	void SetMaxBubbleAmmo(int ammo) { maxBubbleAmmo = ammo; }
	void SetBubbleAmmo(int ammo) { bubbleAmmo = ammo; }

	int GetBubbleRange() const { return bubbleRange; }
	void SetBubbleRange(int range) { bubbleRange = range; }

	void RestoreBubbleAmmo();

protected:
	int lives = 1;
	int maxBubbleAmmo = 1;
	int bubbleAmmo = 1;
	int bubbleRange = 1;
};
