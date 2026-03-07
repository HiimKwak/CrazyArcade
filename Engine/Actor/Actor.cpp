#include <iostream>
#include <Windows.h>

#include "Util/Util.h"
#include "Actor.h"
#include "Render/Renderer.h"
#include "Render/SpriteAsset.h"
#include "Engine/Engine.h"

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
		const int ts = Engine::Get().GetTileSize();
		if (const SpriteAsset* ps = sprite.GetPixelSprite())
		{
			Renderer::Get().SubmitSprite(
				position.x, position.y,
				ps, sortingOrder);
		}
		else
		{
			Renderer::Get().SubmitRect(
				position.x, position.y,
				ts, ts,
				sprite.GetColor(), sortingOrder);
		}
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