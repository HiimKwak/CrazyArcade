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

void Character::NotifyBubbled()
{
	for (auto observer : observers)
	{
		observer->OnCharacterBubbled(this);
	}
}

void Character::NotifyNormal()
{
	for (auto observer : observers)
	{
		observer->OnCharacterNormal(this);
	}
}

void Character::NotifyWaterBalloonExploded(const Vector2& position)
{
	for (auto observer : observers)
	{
		observer->OnWaterBalloonExploded(position);
	}
}

bool Character::QueryCanMove(const Vector2& from, const Vector2& to) const
{
	CharacterCommandDelegate* command = GetDelegate();
	if (!command)
		return false;

	return command->OnQueryCanMove(from, to);
}

Vector2 Character::QueryActorPosition(ActorType type) const
{
	const CharacterQueryDelegate* query = GetQueryDelegate();
	if (!query)
		return Vector2::Zero;

	return query->OnQueryActorPosition(type);
}

Vector2 Character::QueryPlayerPosition() const
{
	return QueryActorPosition(ActorType::Player);
}

bool Character::QueryHasWaterBalloonAt(const Vector2& position) const
{
	const CharacterQueryDelegate* query = GetQueryDelegate();
	if (!query)
		return false;

	return query->OnQueryHasWaterBalloonAt(position);
}

bool Character::QueryHasBoxAt(const Vector2& position) const
{
	const CharacterQueryDelegate* query = GetQueryDelegate();
	if (!query)
		return false;

	return query->OnQueryHasBoxAt(position);
}

bool Character::QueryIsExplosionDangerAt(const Vector2& position) const
{
	const CharacterQueryDelegate* query = GetQueryDelegate();
	if (!query)
		return false;

	return query->OnQueryIsExplosionDangerAt(position);
}

float Character::QueryExplosionTimeAt(const Vector2& position) const
{
	const CharacterQueryDelegate* query = GetQueryDelegate();
	if (!query)
		return std::numeric_limits<float>::infinity();

	return query->OnQueryExplosionTimeAt(position);
}

bool Character::QueryIsActorBubbled(ActorType type) const
{
	const CharacterQueryDelegate* query = GetQueryDelegate();
	if (!query)
		return false;

	return query->OnQueryIsActorBubbled(type);
}

bool Character::QueryIsPlayerBubbled() const
{
	return QueryIsActorBubbled(ActorType::Player);
}

void Character::SetSpriteColor(Color color)
{
	super::SetSpriteColor(color);
}

void Character::ResetSpriteColor()
{
	super::SetSpriteColor(originalColor);
}
