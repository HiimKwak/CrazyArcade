#pragma once

#include <Windows.h>
#include "Common/Common.h"

namespace engine
{
	enum class ENGINE_API Color : unsigned short
	{
		// MS는 BGRA
		Black = 0,
		Blue = FOREGROUND_BLUE, // 1(001) WORD(unsigned short)
		Green = FOREGROUND_GREEN, // 2(010) // 글자는 foreground, 글자 배경은 background
		Red = FOREGROUND_RED, // 4(100)
		White = Blue | Green | Red,
		Skyblue = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		Yellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		Orange = FOREGROUND_RED | FOREGROUND_GREEN,
	};
}