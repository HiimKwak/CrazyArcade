#pragma once

// STL(Standard Template Library)
#include <vector> // 동적 배열
#include "Common/RTTI.h"

namespace Wanted
{
	class Actor;

	// 목표: 레벨에 있는 모든 액터 관리
	class WANTED_API Level : public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI)

	public:
		Level();
		virtual ~Level();

		// game play events
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// add actor to level
		void AddNewActor(Actor* newActor);

		void ProcessAddAndDestoryActors();

	protected:
		std::vector<Actor*> actors;

		// 실행 중에 추가 요청된 액터 배열
		std::vector<Actor*> addRequestedActors;
	};
}

