#pragma once

#include <bitset>

namespace Engine::ecs
{

constexpr std::size_t MAX_COMPONENTS = 64;

using Signature = std::bitset<MAX_COMPONENTS>;

} // namespace Engine::ecs
