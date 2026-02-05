#pragma once

#include "Common/Common.h"

namespace engine
{
	class ENGINE_API Input
	{
		friend class Engine; // 엔진에게만 ProcessInput(private 멤버)을 노출시키고 싶을 때

		struct KeyState // 구조체는 데이터에, 클래스는 기동에 초점
		{
			// key pressed or not on current frame
			bool isKeyDown = false;
			// key pressed or not on previous frame
			bool wasKeyDown = false;
		};

	private: // 생성자를 감췄기 때문에 Engine 클래스 안에서만 생성가능하고, 나머지 범위에선 생성 자체가 막힌다
		Input();
		~Input(); // q. 소멸자를 악용할 가능성?

	public:
		// input checker
		bool GetKeyDown(int keyCode); // called when key pressed newly
		bool GetKeyUp(int keyCode); // called when pressed key canceled 
		bool GetKey(int keyCode); // called when key is being pressed

		static Input& Get();

	private:
		// process input
		void ProcessInput();
		// save previous input states
		void SavePreviousInputStates();

	private:
		// key states
		KeyState keyStates[255] = { };

		static Input* instance;
	};
}

