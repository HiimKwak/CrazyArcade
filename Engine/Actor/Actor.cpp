#include <iostream>
#include <Windows.h>

#include "Util/Util.h"
#include "Actor.h"
#include "Render/Renderer.h"

namespace engine
{
	Actor::Actor(const wchar_t* image, const Vector2& position, Color color)
		: position(position), sprite(image, color)
	{
	}

	Actor::~Actor()
	{

	}

	void Actor::BeginPlay()
	{
		// toggle after receiving begin play call
		hasBegunPlay = true;

		for (auto& component : components)
			component->Initialize(this);
	}

	void Actor::Tick(float deltaTime)
	{
		for (auto& component : components)
			component->Tick(deltaTime);
	}
	void Actor::Draw()
	{
		Renderer::Get().Submit(sprite.GetImage(), position, sprite.GetColor(), sortingOrder);
	}

	void Actor::Destroy(std::function<void()> onDestroy)
	{
		destroyRequested = true;

		if (onDestroy)
			onDestroy();
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		if (position == newPosition)
			return;
		position = newPosition;
	}
}