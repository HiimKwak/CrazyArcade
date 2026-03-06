#pragma once

#include <map>

#include "Actor/Character/Character.h"
#include "Actor/Items/ItemType.h"
#include "Math/Vector2.h"

using namespace engine;

class PlayerController
{
public:
	PlayerController(class Player* owner);
	~PlayerController();

	void Tick(float deltaTime);
	void BindItemToKey(int key, ItemType type);

private:
	void HandleMovementInput(float deltaTime);
	void HandleActionInput();
	void HandleItemInput();

	class Player* owner;
	bool isInputActive = true;
	std::map<int, ItemType> itemKeyBindings;
};
