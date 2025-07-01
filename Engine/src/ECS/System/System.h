#pragma once

#include <vector>

#include "../Entity/Entity.h"

namespace ecs
{
class World;

class System
{

public:
	virtual ~System() = default;

	virtual void Update(World&, float dt) = 0;

	std::vector<Entity> Entities;
};

} // namespace ecs
