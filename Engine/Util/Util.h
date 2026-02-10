#pragma once

#include "Math/Vector2.h"
#include "Math/Color.h"

#include <time.h>

using namespace engine;

// 메모리 정리 함수
template<typename T>
void SafeDelete(T*& t)
{
	if (t)
	{
		delete t;
		t = nullptr;
	}
}

template<typename T>
void SafeDeleteArray(T*& tArray)
{
	if (tArray)
	{
		delete[] tArray;
		tArray = nullptr;
	}
}

namespace Util // Helper
{
	// 콘솔 커서 위치 이동시키는 함수
	inline void SetConsolePosition(const Vector2& position)
	{
		SetConsoleCursorPosition(
			GetStdHandle(STD_OUTPUT_HANDLE),
			static_cast<COORD>(position) // Vector2에 형변환 연산자 오버로딩이 있어야 가능
		);
	}

	// 콘솔 텍스트 설정 함수
	inline void SetConsoleTextColor(Color color)
	{
		SetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE),
			static_cast<unsigned short>(color)
		);
	}

	// turn cursor off
	inline void TurnOffCursor()
	{
		CONSOLE_CURSOR_INFO info = {};
		GetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);

		info.bVisible = false;
		SetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);
	}

	// turn cursor on
	inline void TurnOnCursor()
	{
		CONSOLE_CURSOR_INFO info = {};
		GetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);

		info.bVisible = true;
		SetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);
	}

	inline void SetRandomSeed()
	{
		// 시간 값을 무작위 종자 값으로 설정
		srand(static_cast<unsigned int>(time(nullptr)));
	}

	// 정수 난수 생성기
	inline int Random(int min, int max)
	{
		int diff = (max - min) + 1;
		return ((diff * rand()) / (RAND_MAX + 1)) + min;
	}

	// 부동소수점 난수 생성기
	inline float RandomRange(float min, float max)
	{
		float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float diff = (max - min);
		return (random * diff) + min;
	}

	// 두 수 사이로 고정시키는 클램프
	template<typename T>
	T Clamp(T value, T min, T max)
	{
		if (value < min)
		{
			value = min;
		}
		else if (value > max)
		{
			value = max;
		}

		return value;
	}

	inline void DeepCopyString(wchar_t*& dest, const wchar_t* src)
	{
		SafeDeleteArray(dest);
		size_t length = wcslen(src) + 1;
		dest = new wchar_t[length];
		wcscpy_s(dest, length, src);
	}
}
