#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>

#include "../ComponentArray/ComponentArray.h"
#include "../TypeIndex/TypeIndex.h"

namespace Engine::ecs
{
using ComponentType = TypeIndexType;

class ComponentManager final
{
public:
	template <typename _TComponent>
	void RegisterComponent();

	template <typename _TComponent>
	bool IsComponentRegistered();

	template <typename _TComponent>
	void AddComponent(Entity entity, _TComponent const& component);

	template <typename _TComponent>
	void RemoveComponent(Entity entity);

	template <typename _TComponent>
	_TComponent& GetComponent(Entity entity);

	template <typename _TComponent>
	_TComponent const& GetComponent(Entity entity) const;

	template <typename _TComponent>
	bool HasComponent(Entity entity) const;

	void OnEntityDestroyed(Entity entity);

private:
	std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>> m_componentArrays;

	template <typename _TComponent>
	std::shared_ptr<ComponentArray<_TComponent>> GetComponentArray() const;
};

} // namespace Engine::ecs

#include "ComponentManager.impl"
