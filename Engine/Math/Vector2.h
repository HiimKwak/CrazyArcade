#pragma once

#include <Windows.h>

#include "Common/Common.h"

namespace Wanted
{
	class WANTED_API Vector2
	{
	public:
		Vector2();
		Vector2(int x, int y);
		~Vector2();

		const char* ToString();

		// 편의기능
		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;

		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;

		// 형변환 연산자 오버로딩
		operator COORD() const;

		// 벡터 기본 값
		static Vector2 Zero;
		static Vector2 One;
		static Vector2 Up;
		static Vector2 Right;

	public:
		// x, y 좌표. 콘솔은 정수 좌표만 가능하다
		int x = 0;
		int y = 0;

	private:
		// 벡터 값을 문자열로 변환할 때 사용할 변수
		char* string = nullptr;
	};
}

