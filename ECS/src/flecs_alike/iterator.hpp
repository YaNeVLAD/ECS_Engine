#pragma once
#include "entity.hpp"
#include <cstddef>

namespace ecs
{
template <typename _TVal>
class iterator
{
private:
	const std::vector<_TVal*>& m_values;
	size_t m_index;

public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = _TVal*;
	using difference_type = std::ptrdiff_t;
	using pointer = _TVal**;
	using reference = _TVal&;

	iterator(const std::vector<_TVal*>& values = {}, size_t index = 0)
		: m_values(values)
		, m_index(index)
	{
	}

	reference operator*() const { return *m_values[m_index]; }
	value_type operator->() const { return m_values[m_index]; }

	iterator& operator++()
	{
		++m_index;
		return *this;
	}

	iterator operator++(int)
	{
		iterator tmp = *this;
		++(*this);
		return tmp;
	}

	iterator& operator--()
	{
		--m_index;
		return *this;
	}

	iterator operator--(int)
	{
		iterator tmp = *this;
		--(*this);
		return tmp;
	}

	iterator& operator+=(difference_type n)
	{
		m_index += n;
		return *this;
	}

	iterator& operator-=(difference_type n)
	{
		m_index -= n;
		return *this;
	}

	iterator operator+(difference_type n) const
	{
		iterator tmp = *this;
		tmp.m_index = m_index + n;
		return tmp;
	}

	iterator operator-(difference_type n) const
	{
		iterator tmp = *this;
		tmp.m_index = m_index - n;
		return tmp;
	}

	difference_type operator-(const iterator& other) const
	{
		return static_cast<difference_type>(m_index) - static_cast<difference_type>(other.m_index);
	}

	bool operator==(const iterator& other) const
	{
		return m_index == other.m_index;
	}

	bool operator!=(const iterator& other) const
	{
		return m_index != other.m_index;
	}

	bool operator<(const iterator& other) const { return m_index < other.m_index; }
	bool operator>(const iterator& other) const { return m_index > other.m_index; }
	bool operator<=(const iterator& other) const { return m_index <= other.m_index; }
	bool operator>=(const iterator& other) const { return m_index >= other.m_index; }

	reference operator[](difference_type n) const
	{
		return *(*this + n);
	}
};
} // namespace ecs
