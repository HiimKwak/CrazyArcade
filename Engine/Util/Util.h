#pragma once

#include "Math/Vector2.h"
#include "Math/Color.h"

using namespace Wanted;

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
}

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
