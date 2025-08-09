#pragma once
#include "../Math/Vector.h"
#include <numeric>

namespace Engine::physics::components
{

struct Transform
{
	math::Vector2 Position = { 0.f, 0.f };
};

struct RigidBody
{
	math::Vector2 Velocity = { 0.0f, 0.0f };
	float Mass = 1.0f;
	float Restitution = 0.5f;
	float LinearDamping = 10.f;

	float InvertedMass() const
	{
		if (Mass < std::numeric_limits<float>::epsilon())
		{
			return 0.0f;
		}

		return 1.f / Mass;
	}
};

struct AABBCollider
{
	math::Vector2 Size = { 50.0f, 50.0f };
	math::Vector2 Position = { 0.f, 0.f };
	math::AABB Bounds;

	void MoveBounds(math::Vector2 const& position)
	{
		Position = position;

		math::Vector2 halfSize = Size * 0.5f;
		Bounds.Min = Position - halfSize;
		Bounds.Max = Position + halfSize;
	}
};

struct CollisionManifold
{
	ecs::Entity EntityA = ecs::InvalidEntity;
	ecs::Entity EntityB = ecs::InvalidEntity;

	math::Vector2 Normal;
	float Penetration = 0.f;
};

} // namespace Engine::physics::components
