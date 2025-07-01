#pragma once

#include <cassert>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "IComponentArray.h"

namespace ecs
{

template <typename _TComponent>
class ComponentArray final : public IComponentArray
{
	using ComponentIndex = std::size_t;

public:
	void AddComponent(Entity entity, _TComponent const& component)
	{
		assert(!m_entityToIndex.contains(entity)
			&& "Can't add multiple components of the same type");

		ComponentIndex newIndex = m_components.size();
		m_entityToIndex[entity] = newIndex;
		m_indexToEntity[newIndex] = entity;

		m_components.push_back(component);
	}

	void RemoveComponent(Entity entity)
	{
		assert(m_entityToIndex.contains(entity)
			&& "Entity does not have component to remove");

		size_t removedIndex = m_entityToIndex.at(entity);
		size_t lastIndex = m_components.size() - 1;

		m_components[removedIndex] = std::move(m_components[lastIndex]);

		Entity lastEntity = m_indexToEntity.at(lastIndex);
		m_entityToIndex[lastEntity] = removedIndex;
		m_indexToEntity[removedIndex] = lastEntity;

		m_entityToIndex.erase(entity);
		m_indexToEntity.erase(lastEntity);
		m_components.pop_back();
	}

	_TComponent& GetComponent(Entity entity)
	{
		assert(m_entityToIndex.contains(entity)
			&& "Entity does not have component of this type");

		return m_components[m_entityToIndex.at(entity)];
	}

	_TComponent const& GetComponent(Entity entity) const
	{
		return const_cast<ComponentArray&>(*this).GetComponent(entity);
	}

	bool HasComponent(Entity entity) const
	{
		return m_entityToIndex.contains(entity);
	}

	auto GetComponents()
	{
		return std::views::all(m_components);
	}

	auto GetComponents() const
	{
		return std::views::all(m_components);
	}

	void OnEntityDestroyed(Entity entity) override final
	{
		if (m_entityToIndex.contains(entity))
		{
			RemoveComponent(entity);
		}
	}

private:
	std::vector<_TComponent> m_components;
	std::unordered_map<Entity, ComponentIndex> m_entityToIndex;
	std::unordered_map<ComponentIndex, Entity> m_indexToEntity;
};

} // namespace ecs
