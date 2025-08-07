#pragma once

#include "../src/Math/Vector.h"
#include "../src/Physics/Components.h"
#include "../src/Physics/System.h"

#ifdef ENGINE_FOLD_NAMESPACES
namespace ecs
{
using namespace ecs::math;
using namespace ecs::physics;
using namespace ecs::physics::components;
} // namespace ecs
#endif
