#pragma once

#include <iostream>

#include "../../Engine/src/Script/native/ScriptableEntity.h"

#include "Components.h"

class PlayerController : public ecs::ScriptableEntity
{
public:
	void OnCreate() override
	{
		std::cout << "PlayerController created for entity " << Entity() << std::endl;
	}

	void OnUpdate(float dt) override
	{
		auto& velocity = GetComponent<Velocity>();
		const auto& input = GetComponent<Input>();

		velocity.vx = 0.f;
		velocity.vy = 0.f;

		if (input.moveLeft)
			velocity.vx -= 500.f;
		if (input.moveRight)
			velocity.vx += 500.f;
		if (input.moveUp)
			velocity.vy -= 500.f;
		if (input.moveDown)
			velocity.vy += 500.f;
	}
};
