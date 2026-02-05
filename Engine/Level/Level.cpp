#include "Level.h"
#include "Actor/Actor.h"

namespace engine
{
	Level::Level()
	{
	}
	Level::~Level()
	{
		for (Actor*& actor : actors)
		{
			if (actor)
			{
				delete actor;
				actor = nullptr;
			}
		}

		actors.clear();
	}

	void Level::BeginPlay()
	{
		// BeginPlay must be invoked only once per actor,
		// as actors may be spawned dynamically during gameplay.
		for (Actor* actor : actors)
		{
			if (actor->HasBegunPlay())
			{
				continue;
			}

			actor->BeginPlay();
		}
	}
	void Level::Tick(float deltaTime)
	{
		for (Actor* actor : actors)
		{
			actor->Tick(deltaTime);
		}
	}
	void Level::Draw()
	{
		for (Actor* const actor : actors)
		{
			if (actor->IsActive())
				actor->Draw();
		}
	}
	void Level::AddNewActor(Actor* newActor)
	{
		addRequestedActors.emplace_back(newActor);

		newActor->SetOwner(this);
	}

	void Level::ProcessAddAndDestoryActors()
	{
		for (int ix = 0; ix < static_cast<int>(actors.size());)
		{
			if (actors[ix]->DestroyRequested())
			{
				delete actors[ix];
				actors.erase(actors.begin() + ix);

				continue;
			}

			++ix;
		}

		if (addRequestedActors.size() == 0)
		{
			return;
		}

		for (Actor* const actor : addRequestedActors)
		{
			actors.emplace_back(actor);
		}

		addRequestedActors.clear();
	}
}