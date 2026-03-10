#pragma once

#include "Util/Timer.h"
#include "Actor/Character/Character.h"
#include <vector>

using namespace engine;

class WaterBalloon : public Actor
{
	RTTI_DECLARATIONS(WaterBalloon, Actor)

public:
	WaterBalloon(const Vector2& newPosition, int explosionRange = 1);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	bool IsExploded() const;

	inline Character* GetOwnerCharacter() const { return ownerCharacter; }
	void SetOwnerCharacter(Character* actor) { ownerCharacter = actor; }

	const std::vector<Vector2>& GetPredictedDangerTiles() const { return predictedDangerTiles; }
	float GetRemainingTime() const { return explodeTimer.GetRemainingTime(); }

private:
	void Explode(float deltaTime);
	void ComputeDangerTiles();

	Character* ownerCharacter = nullptr;

	Timer explodeTimer;
	float countDown = 3.0f;
	bool exploded = false;
	int range;

	std::vector<Vector2> predictedDangerTiles;
};
