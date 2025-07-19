#pragma once

namespace ecs
{

using TypeId = std::size_t;

namespace details
{

class TypeIDGenerator final
{
public:
	template <typename _T>
	static TypeId Get()
	{
		static const TypeId id = m_counter++;

		return id;
	}

private:
	static inline TypeId m_counter = 0;
};

} // namespace details

template <typename _T>
TypeId TypeIdOf()
{
	return details::TypeIDGenerator::Get<_T>();
}

template <typename _T>
const char* NameOf()
{
	return typeid(_T).name();
}

} // namespace ecs
