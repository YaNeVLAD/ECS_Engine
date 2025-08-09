#pragma once

#include <cassert>
#include <queue>
#include <vector>

#include "../Entity/Entity.h"
#include "../Entity/Signature.h"

namespace Engine::ecs
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
	TypeIndexType m_nextEntityIndex = 0;
	std::queue<TypeIndexType> m_availableIndices;

	std::vector<TypeIndexType> m_generations;

	std::vector<Signature> m_signatures;

	std::vector<Entity> m_activeEntities;
	std::vector<size_t> m_entityLocations;
};

} // namespace Engine::ecs

#include "EntityManager.impl"
