#pragma once

#include <cstddef>

namespace Engine::ecs::details
{

class TypeIndexGenerator final
{
public:
	template <typename _T>
	static std::size_t Get()
	{
		static const std::size_t id = m_counter++;

		return id;
	}

private:
	inline static std::size_t m_counter = 0;
};

} // namespace Engine::ecs::details

namespace Engine::ecs
{

using TypeIndexType = std::size_t;

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

} // namespace Engine::ecs
