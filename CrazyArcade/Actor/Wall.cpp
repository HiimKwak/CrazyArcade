#include "Wall.h"

Wall::Wall(const Vector2& position) : super(L"бс", position, Color::White)
{
	sortingOrder = 0;
}

