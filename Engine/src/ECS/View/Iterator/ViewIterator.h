#pragma once

#include <tuple>
#include <vector>

#include "../../ComponentManager/ComponentManager.h"

namespace ecs
{

template <bool IsConst, typename... _TComponents>
class ViewIterator final
{
public:
	using EntityIterator = std::conditional_t<IsConst,
		typename std::vector<Entity>::const_iterator,
		typename std::vector<Entity>::iterator>;

	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = std::conditional_t<IsConst,
		std::tuple<Entity, const _TComponents&...>,
		std::tuple<Entity, _TComponents&...>>;

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

	ViewIterator operator++(int)
	{
		ViewIterator tmp = *this;
		++(*this);
		return tmp;
	}

	value_type operator*() const
	{
		Entity entity = *m_it;

		if constexpr (IsConst)
		{
			return std::forward_as_tuple(entity, std::as_const(m_manager).GetComponent<_TComponents>(entity)...);
		}
		else
		{
			return std::forward_as_tuple(entity, m_manager.GetComponent<_TComponents>(entity)...);
		}
	}

	bool operator!=(const ViewIterator& other) const { return m_it != other.m_it; }
	bool operator==(const ViewIterator& other) const { return m_it == other.m_it; }

private:
	ComponentManager& m_manager;
	EntityIterator m_it;
};

} // namespace ecs
