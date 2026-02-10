#include <cstdlib>
#include "Enemy.h"
#include "Actor/Bubble.h"
#include "Level/Level.h"
#include "Util/Util.h"
#include "Interface/IGameRuleManager.h"

using namespace engine;

Enemy::Enemy(const Vector2& position)
	: super(L"Ψ", position, Color::Green)
{
	sortingOrder = 8;

	// 초기 타이머 값
	shootTimer.SetTargetTime(Util::RandomRange(3.0f, 6.0f));
	shootTimer.Reset();

	moveTimer.SetTargetTime(Util::RandomRange(moveIntervalMin, moveIntervalMax));
	moveTimer.Reset();

	// 랜덤 초기 방향
	int r = Util::Random(0, 4);
	switch (r)
	{
	case 0: direction = Vector2(1, 0); break;
	case 1: direction = Vector2(-1, 0); break;
	case 2: direction = Vector2(0, 1); break;
	default: direction = Vector2(0, -1); break;
	}
}

Enemy::~Enemy()
{
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	moveTimer.Tick(deltaTime);
	if (!moveTimer.IsTimeout())
		return;

	IGameRuleManager* level = dynamic_cast<IGameRuleManager*>(GetOwner());
	if (!level)
		return;

	Vector2 nextPosition = GetPosition() + direction;

	if (level->CanMove(GetPosition(), nextPosition))
	{
		SetPosition(nextPosition);
	}
	else
	{
		int r = Util::Random(0, 3);
		switch (r)
		{
		case 0: direction = Vector2(1, 0);
			break;
		case 1: direction = Vector2(-1, 0);
			break;
		case 2: direction = Vector2(0, 1);
			break;
		default: direction = Vector2(0, -1);
			break;
		}
	}

	moveTimer.Reset();

	shootTimer.Tick(deltaTime);
	if (!shootTimer.IsTimeout())
		return;

	if (!level->HasBubbleAt(GetPosition()))
	{
		GetOwner()->AddNewActor(new Bubble(GetPosition()));
	}

	shootTimer.Reset();
	shootTimer.SetTargetTime(Util::RandomRange(3.0f, 6.0f));

}

void Enemy::OnDamaged()
{
	// todo: bubble capsule

	Destroy();

	// todo: effect
}