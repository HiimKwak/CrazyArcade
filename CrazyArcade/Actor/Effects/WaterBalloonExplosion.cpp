#include "Level/Level.h"
#include "../Effects/WaterBalloonExplosion.h"
#include "../Effects/ExplosionTile.h"
#include "Engine/Engine.h"
#include "Interface/IWorldQueryProvider.h"
#include "Interface/IWorldQueryService.h"

WaterBalloonExplosion::WaterBalloonExplosion(const Vector2& origin, int explosionRange)
	: super(L"", origin, Color::White),
	range(explosionRange),
	propagated(false)
{
	sortingOrder = 0;
}

void WaterBalloonExplosion::BeginPlay()
{
	super::BeginPlay();
	Propagate();
}

void WaterBalloonExplosion::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (propagated)
	{
		Destroy();
		propagated = false;
	}
}

void WaterBalloonExplosion::Propagate()
{
	if (propagated) return;

	PropagateInDirection(Vector2::Zero, range);
	PropagateInDirection(Vector2::Up, range);
	PropagateInDirection(Vector2::Right, range);
	PropagateInDirection(Vector2::Down, range);
	PropagateInDirection(Vector2::Left, range);

	propagated = true;
}

void WaterBalloonExplosion::PropagateInDirection(const Vector2& direction, int range)
{
	Vector2 origin = GetPosition();

	for (int i = 1; i <= range; ++i)
	{
		Vector2 tilePos = origin + (i * Engine::Get().GetTileSize()) * direction;

		if (owner)
		{
			IWorldQueryProvider* provider = dynamic_cast<IWorldQueryProvider*>(owner);
			IWorldQueryService* worldQuery = provider ? provider->GetWorldQueryService() : nullptr;

			if (worldQuery)
			{
				if (!worldQuery->IsInsideGameMap(tilePos))
					break;

				if (!worldQuery->CanExplosionPenetrate(tilePos))
					break;
			}
		
			ExplosionTile* tile = new ExplosionTile(tilePos);
			tile->Activate();
			owner->AddNewActor(tile);

			if (worldQuery && worldQuery->HasBoxAt(tilePos))
				break;
		}
	}
}
