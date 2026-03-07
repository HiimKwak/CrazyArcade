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

	// Half-block(?) 렌더링용: 두 픽셀 색상을 CHAR_INFO.Attributes 하나로 합성
	// topColor → foreground (bits 0-3), bottomColor → background (bits 4-7)
	inline WORD MakeHalfBlockAttr(Color topColor, Color bottomColor)
	{
		return static_cast<WORD>(topColor) | (static_cast<WORD>(bottomColor) << 4);
	}
}