#pragma once

#include <vector>

#include "../EntityWrapper/EntityWrapper.h"

namespace Engine::ecs
{
class Scene;

class System
{

public:
	using WrappedEntity = EntityWrapper<Scene>;

	virtual ~System() = default;

	virtual void Update(Scene& scene, float dt) = 0;

	std::vector<WrappedEntity> Entities;
};

} // namespace Engine::ecs
