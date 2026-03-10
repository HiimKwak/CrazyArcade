#pragma once

#include "ICharacterComponent.h"

class WaterBalloonComponent : public ICharacterComponent
{
public:
	virtual void Tick(float deltaTime) override;

	bool RequestGenerateWaterBalloon();
	void OnWaterBalloonExploded();
};
