#pragma once

#include "Common/RTTI.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "Util/Util.h"
#include "IComponent.h"
#include "IController.h"
#include "IDelegate.h"

#include <functional>
#include <memory>
#include <vector>

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

		template<typename T>
		T* GetComponent()
		{
			for (auto& component : components)
			{
				T* comp = dynamic_cast<T*>(component.get());
				if (comp)
					return comp;
			}
			return nullptr;
		}

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			auto component = std::make_unique<T>(std::forward<Args>(args)...);
			T* ptr = component.get();
			components.push_back(std::move(component));
			ptr->Initialize(this);
			return ptr;
		}

		template<typename T, typename... Args>
		T* CreateController(Args&&... args)
		{
			auto ctrl = std::make_unique<T>(std::forward<Args>(args)...);
			T* ptr = ctrl.get();
			controller = std::move(ctrl);
			return ptr;
		}

		IController* GetController() const { return controller.get(); }

		void SetDelegate(IDelegate* newDelegate) { delegate = newDelegate; }
		IDelegate* GetDelegate() const { return delegate; }

		inline Vector2 GetPosition()const { return position; }
		void SetPosition(const Vector2& newPosition);

		inline Level* GetOwner() const { return owner; }
		void SetOwner(Level* newOwner) { owner = newOwner; }

		inline void SetSprite(const wchar_t* src, Color color) { sprite = Sprite(src, color); }
		inline void SetSpriteImage(const wchar_t* src) { return sprite.SetImage(src); }
		inline void SetSpriteColor(Color color) { return sprite.SetColor(color); }

		inline bool HasBegunPlay() const { return hasBegunPlay; }
		inline bool IsActive() const { return isActive && !destroyRequested; }
		inline bool DestroyRequested() const { return destroyRequested; }
		inline int GetSortingOrder() const { return sortingOrder; }

	protected:
		bool hasBegunPlay = false;

		bool isActive = true;

		bool destroyRequested = false;

		Sprite sprite;

		Level* owner = nullptr;

		int sortingOrder = 0;

		std::vector<std::unique_ptr<IComponent>> components;
		std::unique_ptr<IController> controller;

	private:
		Vector2 position;
		IDelegate* delegate = nullptr;
	};
}
