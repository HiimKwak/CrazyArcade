#pragma once

#include "ICharacterComponent.h"

class StatsComponent : public ICharacterComponent
{
public:
	virtual void Tick(float deltaTime) override;

	int GetLives() const { return lives; }
	void SetLives(int newLives) { lives = newLives; }

	int GetMaxWaterBalloonAmmo() const { return maxWaterBalloonAmmo; }
	int GetWaterBalloonAmmo() const { return waterBalloonAmmo; }
	void SetMaxWaterBalloonAmmo(int ammo) { maxWaterBalloonAmmo = ammo; }
	void SetWaterBalloonAmmo(int ammo) { waterBalloonAmmo = ammo; }

	int GetExplosionRange() const { return explosionRange; }
	void SetExplosionRange(int range) { explosionRange = range; }

	void RestoreWaterBalloonAmmo();

protected:
	int lives = 1;
	int maxWaterBalloonAmmo = 1;
	int waterBalloonAmmo = 1;
	int explosionRange = 1;
};
