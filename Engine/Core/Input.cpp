#include <Windows.h>
#include <iostream>

#include "Input.h"

namespace Wanted
{
	// 전역 변수 초기화
	Input* Input::instance = nullptr;

	Input::Input()
	{
		// 객체가 초기화되면 자기 자신의 주소를 저장
		instance = this; // 원래는 엔진에서 관장, 여러번 할당될 경우에 대한 예외처리가 있어야 함. 그러나 이 예제의 경우 통제 가능하므로 생략
	}
	Input::~Input()
	{
	}

	bool Input::GetKeyDown(int keyCode)
	{
		return keyStates[keyCode].isKeyDown &&
			!keyStates[keyCode].wasKeyDown;
	}
	bool Input::GetKeyUp(int keyCode)
	{
		return keyStates[keyCode].wasKeyDown &&
			!keyStates[keyCode].isKeyDown;
	}
	bool Input::GetKey(int keyCode)
	{
		return keyStates[keyCode].isKeyDown;
	}

	Input& Input::Get()
	{
		// SIngleton pattern: 전역적으로 딱 1개 개체만 유지하는 기법
		// 이 함수는 콘텐츠 프로젝트에서 접근함.
		// 따라서 엔진은 이미 초기화된 상태
		if (!instance)
		{
			std::cout << "Error: Input::Get(). instance is null\n";
			__debugbreak(); // 자동 중단점, 디버그 모드에서만 작동
		}
		// Lazy pattern
		// static Input instance; // Get함수 호출시까지 인스턴스 생성 연기
		// return instance; // 게임 업계에선 명확한 주도권을 좋아하기 때문에 비선호
		return *instance;
	}

	void Input::ProcessInput()
	{
		// read each key input
		// must use OS specific API
		for (int ix = 0; ix < 255; ++ix)
		{
			keyStates[ix].isKeyDown
				= (GetAsyncKeyState(ix) & 0x8000) > 0 ? true : false;

		}
	}

	void Input::SavePreviousInputStates()
	{
		// 현재 입력 값을 이전 입력 값으로 저장
		for (int ix = 0; ix < 255; ++ix)
		{
			keyStates[ix].wasKeyDown = keyStates[ix].isKeyDown;
		}
	}

}