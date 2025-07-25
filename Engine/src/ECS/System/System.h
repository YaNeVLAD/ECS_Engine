#pragma once

#include <vector>

#include "../Entity/Entity.h"

namespace ecs
{
class Scene;

class System
{

public:
	virtual ~System() = default;

	virtual void Update(Scene& scene, float dt) = 0;

	std::vector<Entity> Entities;
};

} // namespace ecs
