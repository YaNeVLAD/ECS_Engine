#pragma once

#include <cassert>
#include <concepts>
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
	void AddComponent(Entity entity, _TComponent const& component);

	void RemoveComponent(Entity entity);

	_TComponent& GetComponent(Entity entity);

	_TComponent const& GetComponent(Entity entity) const;

	bool HasComponent(Entity entity) const;

	auto GetComponents();

	auto GetComponents() const;

	void OnEntityDestroyed(Entity entity) override final;

private:
	std::vector<_TComponent> m_components;
	std::unordered_map<Entity, ComponentIndex> m_entityToIndex;
	std::unordered_map<ComponentIndex, Entity> m_indexToEntity;
};

} // namespace ecs

#include "ComponentArray.impl"
