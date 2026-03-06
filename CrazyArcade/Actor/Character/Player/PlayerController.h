#pragma once

#include <map>

#include "Actor/Character/Character.h"
#include "Actor/Character/Component/Item/ItemType.h"
#include "Math/Vector2.h"

using namespace engine;

class PlayerController : public IController
{
public:
	PlayerController(class Player* owner);
	~PlayerController();

	virtual void Tick(float deltaTime) override;
	void BindItemToKey(int key, ItemType type);

private:
	void HandleMovementInput(float deltaTime);
	void HandleActionInput();
	void HandleItemInput();

	class Player* owner;
	bool isInputActive = true;
	std::map<int, ItemType> itemKeyBindings;
};
