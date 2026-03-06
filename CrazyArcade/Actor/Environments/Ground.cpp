#include "Ground.h"

Ground::Ground(const Vector2& position) : super(L"●", position, Color::Black)
{
	sortingOrder = 0;
}