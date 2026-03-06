#pragma once

#include <memory>
#include <vector>

#include "Actor/Actor.h"
#include "Math/Vector2.h"

using namespace engine;

class ICharacterComponent;

class CharacterObserver
{
public:
	virtual ~CharacterObserver() = default;

	virtual void OnCharacterDied(class Character* character) {}
	virtual void OnCharacterBubbleTrapped(class Character* character) {}
	virtual void OnCharacterNormal(class Character* character) {}
	virtual void OnBubbleExploded(const Vector2& position) {}
};

class CharacterDelegate
{
public:
	virtual ~CharacterDelegate() = default;

	virtual bool OnRequestMove(class Character* character, const Vector2& targetPos) = 0;
	virtual bool OnRequestGenerateBubble(class Character* character, const Vector2& position, int range) = 0;

	virtual bool OnQueryCanMove(const Vector2& from, const Vector2& to) = 0;
	virtual Vector2 OnQueryPlayerPosition() = 0;
};

class Character : public Actor
{
	RTTI_DECLARATIONS(Character, Actor)

public:
	Character(const wchar_t* image, const Vector2& position, Color color);
	virtual ~Character();

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

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

	void Subscribe(CharacterObserver* observer);
	void Unsubscribe(CharacterObserver* observer);
	void SetDelegate(CharacterDelegate* newDelegate) { delegate = newDelegate; }
	CharacterDelegate* GetDelegate() const { return delegate; }

	void NotifyDied();
	void NotifyBubbleTrapped();
	void NotifyNormal();
	void NotifyBubbleExploded(const Vector2& position);

	bool QueryCanMove(const Vector2& from, const Vector2& to) const;
	Vector2 QueryPlayerPosition() const;

	void SetSpriteColor(Color color);
	void ResetSpriteColor();

protected:
	Color originalColor;
	std::vector<std::unique_ptr<ICharacterComponent>> components;

private:
	std::vector<CharacterObserver*> observers;
	CharacterDelegate* delegate = nullptr;
};
