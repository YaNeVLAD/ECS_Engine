#pragma once

#include <cassert>
#include <vector>

#include "IComponentArray.h"

namespace Engine::ecs
{

template <typename _TComponent>
class ComponentArray final : public IComponentArray
{
public:
	void AddComponent(Entity entity, _TComponent const& component);

	void RemoveComponent(Entity entity);

	_TComponent& GetComponent(Entity entity);

	_TComponent const& GetComponent(Entity entity) const;

	bool HasComponent(Entity entity) const;

	std::vector<_TComponent>& GetComponents();

	void OnEntityDestroyed(Entity entity) override final;

private:
	static constexpr size_t InvalidIndex = std::numeric_limits<size_t>::max();

	std::vector<_TComponent> m_components;

	std::vector<size_t> m_sparse;

	std::vector<Entity> m_denseToEntity;
};

} // namespace Engine::ecs

#include "ComponentArray.impl"
