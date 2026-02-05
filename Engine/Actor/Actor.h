#pragma once

#include "Common/RTTI.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

namespace Wanted
{
	class Level;

	class WANTED_API Actor : public RTTI // dll 바깥에서 접근가능하게 열어주는 키워드
	{
		// RTTI 코드 추가
		RTTI_DECLARATIONS(Actor, RTTI)

	public:
		Actor(
			const char* image = " ",
			const Vector2& position = Vector2::Zero,
			Color color = Color::White
		);
		virtual ~Actor();

		// game play events
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// 위치 변경 및 읽기
		void SetPosition(const Vector2& newPosition);
		inline Vector2 GetPosition()const { return position; }

		// 오너십 추가/읽기
		void SetOwner(Level* newOwner) { owner = newOwner; }
		inline Level* GetOwner() const { return owner; }

		//getter
		inline bool HasBegunPlay() const
		{
			return hasBegunPlay;
		}
		inline bool IsActive() const
		{
			return isActive && !destroyRequested;
		}
		inline bool DestroyRequested() const
		{
			return destroyRequested;
		}

		inline int GetSortingOrder() const { return sortingOrder; }

	protected:
		// check whether begin play has been called
		bool hasBegunPlay = false;

		// active state
		bool isActive = true;

		// whether received destroy request on current frame
		bool destroyRequested = false;

		// 그려질 문자(이미지)
		char* image = nullptr;

		Color color = Color::White;

		// Ownership
		Level* owner = nullptr;

		// 그리기 우선 순위(z-order/z-index)
		int sortingOrder = 0;

	private:
		// position
		Vector2 position;
	};
}
