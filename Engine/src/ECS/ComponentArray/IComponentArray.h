#pragma once

#include "../Entity/Entity.h"

namespace Engine::ecs
{

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void OnEntityDestroyed(Entity entity) = 0;
};

} // namespace Engine::ecs
