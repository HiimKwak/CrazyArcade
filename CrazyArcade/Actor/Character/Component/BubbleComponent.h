#pragma once

#include "ICharacterComponent.h"

class BubbleComponent : public ICharacterComponent
{
public:
	virtual void Tick(float deltaTime) override;

	bool RequestGenerateBubble();
	void OnBubbleExploded();
};
