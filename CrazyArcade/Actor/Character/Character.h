#pragma once

#include <vector>

#include "Actor/Actor.h"
#include "Math/Vector2.h"
#include "Interface/ActorType.h"

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

class CharacterCommandDelegate : public IDelegate
{
public:
	virtual ~CharacterCommandDelegate() = default;

	virtual bool OnRequestMove(class Character* character, const Vector2& targetPos) = 0;
	virtual bool OnRequestGenerateBubble(class Character* character, const Vector2& position, int range) = 0;
	virtual bool OnQueryCanMove(const Vector2& from, const Vector2& to) = 0;
};

class CharacterQueryDelegate
{
public:
	virtual ~CharacterQueryDelegate() = default;

	virtual Vector2 OnQueryActorPosition(ActorType type) const = 0;
	virtual bool OnQueryHasBubbleAt(const Vector2& position) const = 0;
	virtual bool OnQueryHasBoxAt(const Vector2& position) const = 0;
	virtual bool OnQueryIsExplosionDangerAt(const Vector2& position) const = 0;
	virtual float OnQueryExplosionTimeAt(const Vector2& position) const = 0;
	virtual bool OnQueryIsActorBubbleTrapped(ActorType type) const = 0;
};

class Character : public Actor
{
	RTTI_DECLARATIONS(Character, Actor)

public:
	Character(const wchar_t* image, const Vector2& position, Color color);
	virtual ~Character();

	void Subscribe(CharacterObserver* observer);
	void Unsubscribe(CharacterObserver* observer);
	void SetDelegate(CharacterCommandDelegate* newDelegate) { Actor::SetDelegate(newDelegate); }
	CharacterCommandDelegate* GetDelegate() const { return static_cast<CharacterCommandDelegate*>(Actor::GetDelegate()); }

	void SetQueryDelegate(const CharacterQueryDelegate* newDelegate) { queryDelegate = newDelegate; }
	const CharacterQueryDelegate* GetQueryDelegate() const { return queryDelegate; }

	void NotifyDied();
	void NotifyBubbleTrapped();
	void NotifyNormal();
	void NotifyBubbleExploded(const Vector2& position);

	bool QueryCanMove(const Vector2& from, const Vector2& to) const;
	Vector2 QueryActorPosition(ActorType type) const;
	Vector2 QueryPlayerPosition() const;
	bool QueryHasBubbleAt(const Vector2& position) const;
	bool QueryHasBoxAt(const Vector2& position) const;
	bool QueryIsExplosionDangerAt(const Vector2& position) const;
	float QueryExplosionTimeAt(const Vector2& position) const;
	bool QueryIsActorBubbleTrapped(ActorType type) const;
	bool QueryIsPlayerBubbleTrapped() const;

	void SetSpriteColor(Color color);
	void ResetSpriteColor();

protected:
	Color originalColor;

private:
	std::vector<CharacterObserver*> observers;
	const CharacterQueryDelegate* queryDelegate = nullptr;
};
