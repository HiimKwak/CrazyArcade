#pragma once

#include "Actor/IController.h"
#include "AI/BTNode.h"
#include <memory>

using namespace engine;

class EnemyController : public IController
{
public:
	EnemyController(class Enemy* owner);
	~EnemyController() = default;

	virtual void Tick(float deltaTime) override;

private:
	class Enemy* owner;

	std::unique_ptr<engine::BTNode> movementTree;
	std::unique_ptr<engine::BTNode> actionTree;
};
