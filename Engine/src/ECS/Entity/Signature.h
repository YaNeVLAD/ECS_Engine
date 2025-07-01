#pragma once

#include <bitset>

namespace ecs
{

constexpr std::size_t MAX_COMPONENTS = 64;

using Signature = std::bitset<MAX_COMPONENTS>;

} // namespace ecs
