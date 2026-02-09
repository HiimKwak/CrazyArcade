#include "Box.h"
#include "Item.h"
#include "Util/Util.h"
#include "Level/GameLevel.h"
#include <cstdlib>
#include <ctime>

Box::Box(const Vector2& position)
	: super("бр", position, Color::Yellow)
{
	sortingOrder = 3;
}

void Box::OnDamaged()
{
	if (damaged)
		return;

	damaged = true;

	int posibility = Util::Random(0, 1); // 50%

	if (owner && posibility)
	{
		int randomItem = Util::Random(0, 2);
		ItemType type = static_cast<ItemType>(randomItem);

		owner->AddNewActor(new Item(GetPosition(), type));
	}

	Destroy();
}