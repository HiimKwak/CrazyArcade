#pragma once

#include "Common/RTTI.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

namespace engine
{
	class Level;

	class ENGINE_API Actor : public RTTI
	{
		RTTI_DECLARATIONS(Actor, RTTI)

	public:
		Actor(
			const char* image = " ",
			const Vector2& position = Vector2::Zero,
			Color color = Color::White
		);
		virtual ~Actor();

		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		void Destroy();
		virtual void OnDestroy();

		inline Vector2 GetPosition()const { return position; }
		void SetPosition(const Vector2& newPosition);

		inline Level* GetOwner() const { return owner; }
		void SetOwner(Level* newOwner) { owner = newOwner; }

		inline bool HasBegunPlay() const { return hasBegunPlay; }
		inline bool IsActive() const { return isActive && !destroyRequested; }
		inline bool DestroyRequested() const { return destroyRequested; }
		inline int GetSortingOrder() const { return sortingOrder; }

	protected:
		bool hasBegunPlay = false;

		bool isActive = true;

		bool destroyRequested = false;

		char* image = nullptr;

		Color color = Color::White;

		Level* owner = nullptr;

		int sortingOrder = 0;

	private:
		Vector2 position;
	};
}
