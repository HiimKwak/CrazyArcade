#include "Level/Level.h"
#include "BubbleExplosion.h"
#include "ExplosionTile.h"

BubbleExplosion::BubbleExplosion(const Vector2& origin, int explosionRange)
	: super("", origin, Color::White),
	range(explosionRange),
	propagated(false)
{
	sortingOrder = 0;
}

void BubbleExplosion::BeginPlay()
{
	super::BeginPlay();
	Propagate();
}

void BubbleExplosion::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (propagated)
	{
		Destroy();
		propagated = false;
	}
}

void BubbleExplosion::Propagate()
{
	if (propagated) return;

	PropagateInDirection(Vector2::Zero, range);
	PropagateInDirection(Vector2::Up, range);
	PropagateInDirection(Vector2::Right, range);
	PropagateInDirection(Vector2::Down, range);
	PropagateInDirection(Vector2::Left, range);

	propagated = true;
}

void BubbleExplosion::PropagateInDirection(const Vector2& direction, int range)
{
	Vector2 origin = GetPosition();

	for (int i = 1; i <= range; ++i)
	{
		Vector2 tilePos = origin + (i * direction);

		if (owner)
			owner->AddNewActor(new ExplosionTile(tilePos));
	}
}
