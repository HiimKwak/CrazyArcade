#include <iostream>
#include <Windows.h> // 윈도우의 콘솔 API. 다른 운영체제 API는 헤더가 다르다

#include "Util/Util.h"
#include "Actor.h"
#include "Render/Renderer.h"

namespace Wanted
{
	Actor::Actor(const char* image, const Vector2& position, Color color)
		: position(position), color(color)
	{
		// RAII?
		size_t length = strlen(image) + 1;
		this->image = new char[length];
		strcpy_s(this->image, length, image);
	}

	Actor::~Actor()
	{
		SafeDeleteArray(image);
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
		Renderer::Get().Submit(image, position, color, sortingOrder);
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		// 변경하려는 위치가 현재 위치와 같으면 건너뜀
		if (position == newPosition)
			return;

		// 새로운 위치 설정
		position = newPosition;
	}

}