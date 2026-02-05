#pragma once

#include <Windows.h>

#include "Math/Vector2.h"

namespace Wanted
{
	/*
	* 작성자: 곽민규
	* 날짜: 2026.02.03
	* 내용: 더블 버퍼링에 사용할 Console output 핸들을 관리하는 클래스
	*/
	class ScreenBuffer
	{
	public:
		ScreenBuffer(const Vector2& screenSize);
		~ScreenBuffer();

		// 콘솔 버퍼 클리어
		void Clear();

		// 콘솔에 2차원 글자 배열 그리기
		void Draw(CHAR_INFO* charInfo);

		// 버퍼 반환 getter
		inline HANDLE GetBuffer() const { return buffer; }

	private:
		// 콘솔 출력 핸들
		HANDLE buffer = nullptr;
		// 화면 크기
		Vector2 screenSize;
	};
}

