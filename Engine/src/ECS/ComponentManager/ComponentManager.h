#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>

#include "../ComponentArray/ComponentArray.h"
#include "../TypeId/TypeId.h"

namespace ecs
{
using ComponentType = TypeId;

class ComponentManager final
{
public:
	template <typename _TComponent>
	void RegisterComponent()
	{
		ComponentType componentType = TypeIdOf<_TComponent>();

		assert(!m_componentArrays.contains(componentType)
			&& "Can't register the same component more than once");

		m_componentArrays[componentType] = std::make_shared<ComponentArray<_TComponent>>();
	}

	template <typename _TComponent>
	void AddComponent(Entity entity, _TComponent const& component)
	{
		GetComponentArray<_TComponent>()->AddComponent(entity, component);
	}

	template <typename _TComponent>
	void RemoveComponent(Entity entity)
	{
		GetComponentArray<_TComponent>()->RemoveComponent(entity);
	}

	template <typename _TComponent>
	_TComponent& GetComponent(Entity entity)
	{
		return GetComponentArray<_TComponent>()->GetComponent(entity);
	}

	template <typename _TComponent>
	_TComponent const& GetComponent(Entity entity) const
	{
		return GetComponentArray<_TComponent>()->GetComponent(entity);
	}

	template <typename _TComponent>
	bool HasComponent(Entity entity) const
	{
		return GetComponentArray<_TComponent>()->HasComponent(entity);
	}

	void OnEntityDestroyed(Entity entity)
	{
		for (auto const& [type, array] : m_componentArrays)
		{
			array->OnEntityDestroyed(entity);
		}
	}

private:
	std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>> m_componentArrays;

	template <typename _TComponent>
	std::shared_ptr<ComponentArray<_TComponent>> GetComponentArray() const
	{
		ComponentType componentType = TypeIdOf<_TComponent>();

		assert(m_componentArrays.contains(componentType)
			&& "Component is not registered");

		return std::static_pointer_cast<ComponentArray<_TComponent>>(m_componentArrays.at(componentType));
	}
};

} // namespace ecs
