#pragma once

#include <vector>
#include "Actor/Actor.h"
#include "Actor/Effects/ExplosionTile.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Enemy/Enemy.h"
#include "Actor/Environments/Box.h"
#include "Actor/Character/Component/State/StateComponent.h"
#include "Actor/Character/Component/Item/ItemComponent.h"

using namespace engine;

class ActorCollisionSystem
{
public:
	void BindActors(const std::vector<Actor*>* inActors)
	{
		actors = inActors;
	}

	void ProcessCollisions()
	{
		if (!actors) return;

		std::vector<ExplosionTile*> activeExplosions;
		Player* player = nullptr;
		std::vector<Enemy*> enemies;
		std::vector<Box*> boxes;

		CollectTargets(activeExplosions, player, enemies, boxes);
		ResolveExplosionHits(activeExplosions, player, enemies, boxes);
		ResolveCharacterContacts(player, enemies);
	}

private:
	void CollectTargets(
		std::vector<ExplosionTile*>& activeExplosions,
		Player*& player,
		std::vector<Enemy*>& enemies,
		std::vector<Box*>& boxes) const
	{
		for (Actor* actor : *actors)
		{
			if (actor->IsTypeOf<ExplosionTile>())
			{
				ExplosionTile* seg = actor->As<ExplosionTile>();
				if (seg->IsActive())
					activeExplosions.push_back(seg);
			}

			if (actor->IsTypeOf<Player>())
				player = actor->As<Player>();

			if (actor->IsTypeOf<Enemy>())
				enemies.push_back(actor->As<Enemy>());

			if (actor->IsTypeOf<Box>())
				boxes.push_back(actor->As<Box>());
		}
	}

	void ResolveExplosionHits(
		const std::vector<ExplosionTile*>& activeExplosions,
		Player* player,
		const std::vector<Enemy*>& enemies,
		const std::vector<Box*>& boxes) const
	{
		for (ExplosionTile* explosion : activeExplosions)
		{
			Vector2 explosionPos = explosion->GetPosition();

			if (player && player->GetPosition() == explosionPos)
			{
				auto stateComp = player->GetComponent<StateComponent>();
				auto itemComp = player->GetComponent<ItemComponent>();
				if (stateComp && stateComp->GetCurrentState() == StateType::Normal)
				{
					if (itemComp && itemComp->HasActiveShield())
						continue;

					stateComp->ChangeState(StateType::Bubbled);
				}
			}

			for (Enemy* enemy : enemies)
			{
				if (enemy->GetPosition() != explosionPos) continue;
				auto stateComp = enemy->GetComponent<StateComponent>();
				if (stateComp && stateComp->GetCurrentState() == StateType::Normal)
					stateComp->ChangeState(StateType::Bubbled);
			}

			for (Box* box : boxes)
			{
				if (box->GetPosition() == explosionPos)
					box->OnDamaged();
			}
		}
	}

	void ResolveCharacterContacts(Player* player, const std::vector<Enemy*>& enemies) const
	{
		if (!player) return;

		for (Enemy* enemy : enemies)
		{
			if (enemy->GetPosition() != player->GetPosition())
				continue;

			auto playerState = player->GetComponent<StateComponent>();
			auto enemyState = enemy->GetComponent<StateComponent>();
			if (!playerState || !enemyState || playerState->IsDead() || enemyState->IsDead())
				continue;

			if (playerState->IsBubbled())
				player->OnKilled();
			else if (enemyState->IsBubbled())
				enemy->OnDamaged();
		}
	}

private:
	const std::vector<Actor*>* actors = nullptr;
};
