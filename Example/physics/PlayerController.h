#pragma once

#include <iostream>

#include <physics.hpp>
#include <scripts.hpp>

struct Input
{
	bool moveLeft = false;
	bool moveRight = false;
	bool moveUp = false;
	bool moveDown = false;
};

class PlayerController : public Engine::scripts::ScriptableEntity
{
public:
	void OnCreate() override
	{
		std::cout << "PlayerController created for entity " << Entity() << std::endl;
	}

	void OnUpdate(float) override
	{
		auto& body = GetComponent<Engine::physics::components::RigidBody>();
		const auto& input = GetComponent<Input>();

		body.Velocity.X = 0.f;
		body.Velocity.Y = 0.f;

		if (input.moveLeft)
			body.Velocity.X -= 500.f;
		if (input.moveRight)
			body.Velocity.X += 500.f;
		if (input.moveUp)
			body.Velocity.Y += 500.f;
		if (input.moveDown)
			body.Velocity.Y -= 500.f;
	}
};
