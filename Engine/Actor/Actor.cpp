#include <iostream>
#include <Windows.h> // 윈도우의 콘솔 API. 다른 운영체제 API는 헤더가 다르다

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
		// 1. Actor의 소멸자 본문 실행
		// 2. 멤버 변수들의 소멸자가 역순으로 자동 호출
		//    -> sprite.~Sprite() 자동 호출
		//    -> position.~Vector2() 자동 호출
	}

	void Actor::BeginPlay()
	{
		// toggle after receiving begin play call
		hasBegunPlay = true;
	}
	void Actor::Tick(float deltaTime)
	{
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

		// 새로운 위치 설정
		position = newPosition;
	}
}