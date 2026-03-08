#pragma once

#include <vector>

#include "Actor/Actor.h"
#include "Math/Vector2.h"

using namespace engine;

class CharacterObserver
{
public:
	virtual ~CharacterObserver() = default;

	virtual void OnCharacterDied(class Character* character) {}
	virtual void OnCharacterBubbleTrapped(class Character* character) {}
	virtual void OnCharacterNormal(class Character* character) {}
	virtual void OnBubbleExploded(const Vector2& position) {}
};

class CharacterDelegate : public IDelegate
{
public:
	virtual ~CharacterDelegate() = default;

	virtual bool OnRequestMove(class Character* character, const Vector2& targetPos) = 0;
	virtual bool OnRequestGenerateBubble(class Character* character, const Vector2& position, int range) = 0;

	virtual bool OnQueryCanMove(const Vector2& from, const Vector2& to) = 0;
	virtual Vector2 OnQueryPlayerPosition() = 0;
	virtual bool OnQueryHasBubbleAt(const Vector2& position) = 0;
	virtual bool OnQueryHasBoxAt(const Vector2& position) = 0;
	virtual bool OnQueryIsExplosionDangerAt(const Vector2& position) = 0;
};

class Character : public Actor
{
	RTTI_DECLARATIONS(Character, Actor)

public:
	Character(const wchar_t* image, const Vector2& position, Color color);
	virtual ~Character();

	void Subscribe(CharacterObserver* observer);
	void Unsubscribe(CharacterObserver* observer);
	void SetDelegate(CharacterDelegate* newDelegate) { Actor::SetDelegate(newDelegate); }
	CharacterDelegate* GetDelegate() const { return static_cast<CharacterDelegate*>(Actor::GetDelegate()); }

	void NotifyDied();
	void NotifyBubbleTrapped();
	void NotifyNormal();
	void NotifyBubbleExploded(const Vector2& position);

	bool QueryCanMove(const Vector2& from, const Vector2& to) const;
	Vector2 QueryPlayerPosition() const;
	bool QueryHasBubbleAt(const Vector2& position) const;
	bool QueryHasBoxAt(const Vector2& position) const;
	bool QueryIsExplosionDangerAt(const Vector2& position) const;

	void SetSpriteColor(Color color);
	void ResetSpriteColor();

protected:
	Color originalColor;

private:
	std::vector<CharacterObserver*> observers;
};
