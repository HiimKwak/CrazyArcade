#pragma once

#include <vector>
#include "Common/RTTI.h"

namespace engine
{
	class Actor;

	class ENGINE_API Level : public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI)

	public:
		Level();
		virtual ~Level();

		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		void AddNewActor(Actor* newActor);

		void ProcessAddAndDestoryActors();

	protected:
		std::vector<Actor*> actors;
		std::vector<Actor*> addRequestedActors;
	};
}

