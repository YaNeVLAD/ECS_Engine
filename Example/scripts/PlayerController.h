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

		velocity.vel.x = 0.f;
		velocity.vel.y = 0.f;

		if (input.moveLeft)
			velocity.vel.x -= 500.f;
		if (input.moveRight)
			velocity.vel.x += 500.f;
		if (input.moveUp)
			velocity.vel.y -= 500.f;
		if (input.moveDown)
			velocity.vel.y += 500.f;
	}
};
