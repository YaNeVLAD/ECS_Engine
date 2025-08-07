#pragma once

#include <cassert>
#include <queue>
#include <vector>

#include "../Entity/Entity.h"
#include "../Entity/Signature.h"

namespace ecs
{

class EntityManager final
{
public:
	[[nodiscard]] Entity CreateEntity();

	void DestroyEntity(Entity entity);

	void SetSignature(Entity entity, Signature const& signature);

	Signature& GetSignature(Entity entity);

	Signature const& GetSignature(Entity entity) const;

	[[nodiscard]] std::vector<Entity> const& GetActiveEntities() const;

private:
	Entity m_nextEntity = 0;
	std::queue<Entity> m_availableEntities;
	std::vector<Signature> m_signatures;

	std::vector<Entity> m_activeEntities;
	std::vector<size_t> m_entityLocations;
};

} // namespace ecs

#include "EntityManager.impl"
