#pragma once

#include "../ECS/Scene/Scene.h"
#include "../ECS/System/System.h"

#include "Components.h"

namespace ecs::physics
{

class PhysicsSystem : public ecs::System
{
public:
	void Update(ecs::Scene& scene, float dt) override
	{
		using namespace ecs::math;
		using namespace ecs::physics::components;

		for (auto& entity : Entities)
		{
			auto& transform = entity.GetComponent<Transform>();
			auto& rigidBody = entity.GetComponent<RigidBody>();
			auto& collider = entity.GetComponent<AABBCollider>();

			rigidBody.Velocity *= (1.0f - dt * rigidBody.LinearDamping);

			transform.Position += rigidBody.Velocity * dt;
			collider.MoveBounds(transform.Position);
		}

		std::vector<CollisionManifold> collisions;
		for (size_t i = 0; i < Entities.size(); ++i)
		{
			for (size_t j = i + 1; j < Entities.size(); ++j)
			{
				auto& entityA = Entities[i];
				auto& entityB = Entities[j];

				const auto& colliderA = entityA.GetComponent<AABBCollider>();
				const auto& colliderB = entityB.GetComponent<AABBCollider>();

				if (colliderA.Bounds.Intersects(colliderB.Bounds))
				{
					collisions.emplace_back(CreateManifold(entityA, entityB));
				}
			}
		}

		for (const auto& manifold : collisions)
		{
			ResolveCollision(scene, manifold);
		}
	}

private:
	components::CollisionManifold CreateManifold(
		ecs::System::WrappedEntity const& entityA,
		ecs::System::WrappedEntity const& entityB)
	{
		using namespace ecs::math;
		using namespace ecs::physics::components;

		const auto& trA = entityA.GetComponent<Transform>();
		const auto& trB = entityB.GetComponent<Transform>();
		const auto& colA = entityA.GetComponent<AABBCollider>();
		const auto& colB = entityB.GetComponent<AABBCollider>();

		Vector2 n = trB.Position - trA.Position;
		float x_overlap = (colA.Size.X / 2.f) + (colB.Size.X / 2.f) - std::abs(n.X);
		float y_overlap = (colA.Size.Y / 2.f) + (colB.Size.Y / 2.f) - std::abs(n.Y);

		CollisionManifold manifold;
		manifold.EntityA = entityA.GetEntity();
		manifold.EntityB = entityB.GetEntity();

		if (x_overlap < y_overlap)
		{
			manifold.Penetration = x_overlap;
			manifold.Normal = (n.X > 0) ? Vector2{ -1.0f, 0.0f } : Vector2{ 1.0f, 0.0f };
		}
		else
		{
			manifold.Penetration = y_overlap;
			manifold.Normal = (n.Y > 0) ? Vector2{ 0.0f, -1.0f } : Vector2{ 0.0f, 1.0f };
		}
		return manifold;
	}

	void ResolveCollision(ecs::Scene& world, components::CollisionManifold const& manifold)
	{
		using namespace ecs::math;
		using namespace ecs::physics::components;

		auto& rbA = world.GetComponent<RigidBody>(manifold.EntityA);
		auto& rbB = world.GetComponent<RigidBody>(manifold.EntityB);
		auto& trA = world.GetComponent<Transform>(manifold.EntityA);
		auto& trB = world.GetComponent<Transform>(manifold.EntityB);

		float invMassA = rbA.InvertedMass();
		float invMassB = rbB.InvertedMass();

		Vector2 rv = rbB.Velocity - rbA.Velocity;
		float velAlongNormal = Dot(rv, manifold.Normal);

		if (velAlongNormal > 0)
			return;

		float e = std::min(rbA.Restitution, rbB.Restitution);
		float j = -(1 + e) * velAlongNormal;
		j /= invMassA + invMassB;

		Vector2 impulse = j * manifold.Normal;

		rbA.Velocity += impulse * invMassA;
		rbB.Velocity -= impulse * invMassB;

		const float percent = 0.2f;
		const float slop = 0.01f;
		Vector2 correction = std::max(manifold.Penetration - slop, 0.0f)
			/ (invMassA + invMassB) * percent * manifold.Normal;

		trA.Position += correction * invMassA;
		trB.Position -= correction * invMassB;
	}
};

} // namespace ecs::physics
