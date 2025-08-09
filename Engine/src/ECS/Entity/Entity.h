#pragma once
#include <cstddef>
#include <cstdint>

namespace ecs::details
{
constexpr std::size_t ENTITY_INDEX_BITS = 32;
constexpr std::size_t ENTITY_GENERATION_BITS = 32;

constexpr std::size_t ENTITY_INDEX_MASK = (1ULL << ENTITY_INDEX_BITS) - 1;
constexpr std::size_t ENTITY_GENERATION_MASK = (1ULL << ENTITY_GENERATION_BITS) - 1;
} // namespace ecs::details

namespace ecs
{

using IdType = std::uint32_t;
using GenerationType = std::uint32_t;

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

inline Entity CreateEntity(std::uint32_t idx, std::uint32_t generation)
{
	std::size_t id = (generation >> details::ENTITY_INDEX_BITS) | idx;
	return Entity{ idx };
}

constexpr std::size_t InvalidEntity = std::size_t(-1);

} // namespace ecs
