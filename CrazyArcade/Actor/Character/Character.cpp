#include "Character.h"
#include <limits>

using namespace engine;

Character::Character(const wchar_t* image, const Vector2& position, Color color)
	: Actor(image, position, color), originalColor(color)
{
}

Character::~Character()
{
	for (auto observer : observers)
	{
		observer = nullptr;
	}
	observers.clear();
}

void Character::Subscribe(CharacterObserver* observer)
{
	if (observer)
	{
		observers.push_back(observer);
	}
}

void Character::Unsubscribe(CharacterObserver* observer)
{
	observers.erase(
		std::remove(observers.begin(), observers.end(), observer),
		observers.end()
	);
}

void Character::NotifyDied()
{
	for (auto observer : observers)
	{
		observer->OnCharacterDied(this);
	}
}

void Character::NotifyBubbleTrapped()
{
	for (auto observer : observers)
	{
		observer->OnCharacterBubbleTrapped(this);
	}
}

void Character::NotifyNormal()
{
	for (auto observer : observers)
	{
		observer->OnCharacterNormal(this);
	}
}

void Character::NotifyBubbleExploded(const Vector2& position)
{
	for (auto observer : observers)
	{
		observer->OnBubbleExploded(position);
	}
}

bool Character::QueryCanMove(const Vector2& from, const Vector2& to) const
{
	CharacterDelegate* charDelegate = GetDelegate();
	if (!charDelegate)
		return false;

	return charDelegate->OnQueryCanMove(from, to);
}

Vector2 Character::QueryPlayerPosition() const
{
	CharacterDelegate* charDelegate = GetDelegate();
	if (!charDelegate)
		return Vector2::Zero;

	return charDelegate->OnQueryPlayerPosition();
}

bool Character::QueryHasBubbleAt(const Vector2& position) const
{
	CharacterDelegate* charDelegate = GetDelegate();
	if (!charDelegate)
		return false;

	return charDelegate->OnQueryHasBubbleAt(position);
}

bool Character::QueryHasBoxAt(const Vector2& position) const
{
	CharacterDelegate* charDelegate = GetDelegate();
	if (!charDelegate)
		return false;

	return charDelegate->OnQueryHasBoxAt(position);
}

bool Character::QueryIsExplosionDangerAt(const Vector2& position) const
{
	CharacterDelegate* charDelegate = GetDelegate();
	if (!charDelegate)
		return false;

	return charDelegate->OnQueryIsExplosionDangerAt(position);
}

float Character::QueryExplosionTimeAt(const Vector2& position) const
{
	CharacterDelegate* charDelegate = GetDelegate();
	if (!charDelegate)
		return std::numeric_limits<float>::infinity();

	return charDelegate->OnQueryExplosionTimeAt(position);
}

bool Character::QueryIsPlayerBubbleTrapped() const
{
	CharacterDelegate* charDelegate = GetDelegate();
	if (!charDelegate)
		return false;

	return charDelegate->OnQueryIsPlayerBubbleTrapped();
}

void Character::SetSpriteColor(Color color)
{
	super::SetSpriteColor(color);
}

void Character::ResetSpriteColor()
{
	super::SetSpriteColor(originalColor);
}
