#include "Wall.h"

Wall::Wall(const Vector2& position) : super(L"бс", position, Color::White)
{
	sortingOrder = 0;
}

Wall::Wall(const Vector2& position, bool visible)
	: super(L"бс", position, Color::White), isVisible(visible)
{
	sortingOrder = 0;
}

void Wall::Draw()
{
	if (isVisible)
		super::Draw();
}