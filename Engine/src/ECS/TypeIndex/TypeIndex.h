#pragma once

#include <cstddef>

namespace ecs
{

using TypeIndexType = std::size_t;

namespace details
{

class TypeIndexGenerator final
{
public:
	template <typename _T>
	static TypeIndexType Get()
	{
		static const TypeIndexType id = m_counter++;

		return id;
	}

private:
	inline static TypeIndexType m_counter = 0;
};

} // namespace details

template <typename _T>
TypeIndexType TypeIndex()
{
	return details::TypeIndexGenerator::Get<_T>();
}

template <typename _T>
const char* Name()
{
	return typeid(_T).name();
}

} // namespace ecs
