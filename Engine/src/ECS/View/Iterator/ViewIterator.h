#pragma once

#include <tuple>
#include <vector>

#include "../../ComponentManager/ComponentManager.h"

namespace ecs
{
template <typename... _TComponents>
class ViewIterator
{
	using EntityIterator = typename std::vector<Entity>::iterator;

public:
	ViewIterator(ComponentManager& manager, EntityIterator it)
		: m_manager(manager)
		, m_it(it)
	{
	}

	ViewIterator& operator++()
	{
		++m_it;
		return *this;
	}

	std::tuple<Entity, _TComponents...> operator*() const
	{
		Entity entity = *m_it;
		return std::tie(entity, m_manager.GetComponent<_TComponents>(entity)...);
	}

	bool operator!=(ViewIterator const& other)
	{
		return m_it != other.m_it;
	}

private:
	ComponentManager& m_manager;
	EntityIterator m_it;
};
} // namespace ecs
