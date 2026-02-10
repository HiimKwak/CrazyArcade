#include <iostream>

#include "Vector2.h"

namespace engine
{
	Vector2 Vector2::Zero(0, 0);
	Vector2 Vector2::One(1, 1);
	Vector2 Vector2::Up(0, 1);
	Vector2 Vector2::Right(1, 0);
	Vector2 Vector2::Down(0, -1);
	Vector2 Vector2::Left(-1, 0);

	Vector2::Vector2()
	{
	}

	Vector2::Vector2(int x, int y)
		: x(x), y(y) // 초기화 시점에 매개변수로 아예 설정해버림
	{
		// 이미 0, 0으로 초기화된 값을 다시 매개변수로 설정함 = 손해
		//this->x = x;
		//this->y = y;
	}

	Vector2::~Vector2()
	{
		if (string)
		{
			delete[] string;
			string = nullptr;
		}
	}

	const wchar_t* Vector2::ToString()
	{
		// 기존 문자열이 있다면 제거
		if (string)
		{
			delete[] string;
			string = nullptr;
		}

		string = new wchar_t[128];
		memset(string, 0, sizeof(wchar_t) * 128);
		swprintf_s(string, 128, L"(%d, %d)", x, y);

		return string;
	}

	Vector2 Vector2::operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 Vector2::operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}

	bool Vector2::operator==(const Vector2& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	bool Vector2::operator!=(const Vector2& other) const
	{
		return !(*this == other);
	}

	Vector2 Vector2::operator*(int scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}

	Vector2::operator COORD() const
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}

	Vector2 operator*(int scalar, const Vector2& v)
	{
		return Vector2(v.x * scalar, v.y * scalar);
	}

	Vector2 Vector2::Lerp(const Vector2& a, const Vector2& b, float t)
	{
		return Vector2(
			a.x + static_cast<int>((b.x - a.x) * t),
			a.y + static_cast<int>((b.y - a.y) * t)
		);
	}
}

