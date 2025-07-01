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
	[[nodiscard]] Entity CreateEntity()
	{
		Entity entity;

		if (!m_availableEntities.empty())
		{
			entity = m_availableEntities.front();
			m_availableEntities.pop();
		}
		else
		{
			entity = m_nextEntity++;
		}

		if (entity >= m_signatures.size())
		{
			m_signatures.resize(entity + 1);
		}

		m_signatures[entity].reset();

		return entity;
	}

	void DestroyEntity(Entity entity)
	{
		assert(entity < m_nextEntity && "Entity out of range");

		m_signatures[entity].reset();

		m_availableEntities.push(entity);
	}

	void SetSignature(Entity entity, Signature const& signature)
	{
		assert(entity < m_nextEntity && "Entity out of range");

		m_signatures[entity] = signature;
	}

	Signature& GetSignature(Entity entity)
	{
		assert(entity < m_nextEntity && "Entity out of range");

		return m_signatures[entity];
	}

	Signature const& GetSignature(Entity entity) const
	{
		return const_cast<EntityManager&>(*this).GetSignature(entity);
	}

private:
	Entity m_nextEntity = 0;
	std::queue<Entity> m_availableEntities;
	std::vector<Signature> m_signatures;
};

} // namespace ecs
