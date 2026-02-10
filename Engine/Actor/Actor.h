#pragma once

#include "Common/RTTI.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "Util/Util.h"

#include <functional>

namespace engine
{
	class Level;

	class ENGINE_API Actor : public RTTI
	{
		struct Sprite
		{
		public:
			Sprite() = default;
			Sprite(const wchar_t* src, Color color)
			{
				Util::DeepCopyString(image, src);
				this->color = color;
			}
			Sprite(const Sprite& other)
			{
				Util::DeepCopyString(image, other.image);
				color = other.color;
			}
			Sprite& operator=(const Sprite& other)
			{
				if (this != &other)
				{
					SafeDeleteArray(image);
					color = other.color;
					if (other.image)
						Util::DeepCopyString(image, other.image);
				}
				return *this;
			}
			~Sprite()
			{
				SafeDeleteArray(image);
			}

			inline wchar_t* GetImage() const { return image; }
			inline void SetImage(const wchar_t* src) { Util::DeepCopyString(image, src); }
			inline Color GetColor() const { return color; }
			inline void SetColor(Color newColor) { color = newColor; }

		private:
			wchar_t* image = nullptr;
			Color color = Color::White;
		};

		RTTI_DECLARATIONS(Actor, RTTI)

	public:
		Actor(
			const wchar_t* image = L" ",
			const Vector2& position = Vector2::Zero,
			Color color = Color::White
		);
		virtual ~Actor();

		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		void Destroy(std::function<void()> onDestroy = nullptr);

		inline Vector2 GetPosition()const { return position; }
		void SetPosition(const Vector2& newPosition);

		inline Level* GetOwner() const { return owner; }
		void SetOwner(Level* newOwner) { owner = newOwner; }

		void SetSprite(const wchar_t* src, Color color);

		inline bool HasBegunPlay() const { return hasBegunPlay; }
		inline bool IsActive() const { return isActive && !destroyRequested; }
		inline bool DestroyRequested() const { return destroyRequested; }
		inline int GetSortingOrder() const { return sortingOrder; }

	protected:
		bool hasBegunPlay = false;

		bool isActive = true;

		bool destroyRequested = false;

		Sprite sprite; // Actor가 스택에 생성되면 Sprite도 스택에, 힙에 생성되면 Sprite도 힙에 생성된다

		Level* owner = nullptr;

		int sortingOrder = 0;

	private:
		Vector2 position;
	};
}
