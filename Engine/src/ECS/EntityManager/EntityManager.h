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
	bool IsValid(Entity entity) const;

private:
	IdType m_nextEntityIndex = 0;
	std::queue<IdType> m_availableIndices;

	std::vector<IdType> m_generations;

	std::vector<Signature> m_signatures;

	std::vector<Entity> m_activeEntities;
	std::vector<size_t> m_entityLocations;
};

} // namespace ecs

#include "EntityManager.impl"
