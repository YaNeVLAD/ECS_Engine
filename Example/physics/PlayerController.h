#pragma once

#include <iostream>

#include <Script/native/ScriptableEntity.h>

#include <Physics/Components.h>

struct Input
{
	bool moveLeft = false;
	bool moveRight = false;
	bool moveUp = false;
	bool moveDown = false;
};

class PlayerController : public ecs::ScriptableEntity
{
public:
	void OnCreate() override
	{
		std::cout << "PlayerController created for entity " << Entity() << std::endl;
	}

	void OnUpdate(float) override
	{
		auto& body = GetComponent<ecs::physics::components::RigidBody>();
		const auto& input = GetComponent<Input>();

		body.Velocity.X = 0.f;
		body.Velocity.Y = 0.f;

		if (input.moveLeft)
			body.Velocity.X -= 500.f;
		if (input.moveRight)
			body.Velocity.X += 500.f;
		if (input.moveUp)
			body.Velocity.Y -= 500.f;
		if (input.moveDown)
			body.Velocity.Y += 500.f;
	}
};
