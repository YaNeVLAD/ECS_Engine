#pragma once

namespace details
{

template <typename _TNative>
class SimpleType final
{
public:
	SimpleType() = default;

	template <typename _R>
	SimpleType(SimpleType<_R> const& other)
		: m_value(static_cast<_R>(other))
	{
	}

	template <typename _R>
	SimpleType(SimpleType<_R>&& other)
		: m_value(std::move(static_cast<_R>(other)))
	{
	}

	SimpleType(_TNative value)
		: m_value(std::move(value))
	{
	}

	_TNative Value()
	{
		return m_value;
	}

	_TNative Value() const
	{
		return m_value;
	}

	operator _TNative()
	{
		return m_value;
	}

	operator _TNative() const
	{
		return m_value;
	}

	template <typename _R>
	SimpleType& operator=(SimpleType<_R> const& other)
	{
		m_value = other.m_value;
		return *this;
	}

	template <typename _R>
	SimpleType& operator=(SimpleType<_R>&& other)
	{
		m_value = std::move(other.m_value);
		return *this;
	}

	template <typename _R>
	auto operator<=>(SimpleType<_R> const& other) const
	{
		return this <=> other;
	}

private:
	_TNative m_value;
};

} // namespace details
