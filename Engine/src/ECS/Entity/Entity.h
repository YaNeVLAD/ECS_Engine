#pragma once
#include <cstddef>

namespace ecs::details
{

constexpr std::size_t ENTITY_INDEX_BITS = 32;
constexpr std::size_t ENTITY_GENERATION_BITS = 32;

constexpr std::size_t ENTITY_INDEX_MASK = (1ULL << ENTITY_INDEX_BITS) - 1;
constexpr std::size_t ENTITY_GENERATION_MASK = (1ULL << ENTITY_GENERATION_BITS) - 1;

} // namespace ecs::details

namespace ecs
{

struct Entity
{
	std::size_t id;

	operator std::size_t()
	{
		return Index();
	}

	operator std::size_t() const
	{
		return Index();
	}

	std::size_t Index() const
	{
		return id & details::ENTITY_INDEX_MASK;
	}

	std::size_t Generation() const
	{
		return (id >> details::ENTITY_INDEX_BITS) & details::ENTITY_GENERATION_MASK;
	}

	auto operator<=>(Entity const&) const = default;
};

inline Entity CreateEntity(std::size_t index, std::size_t generation)
{
	return Entity{ (generation << details::ENTITY_INDEX_BITS) | index };
}

constexpr Entity InvalidEntity = Entity{ details::ENTITY_INDEX_MASK };

} // namespace ecs
