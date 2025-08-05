#pragma once

#include "ScriptComponent.h"

#include "../../ECS/Scene/Scene.h"

namespace ecs
{

class ScriptingSystem : public System
{
public:
	void Update(ecs::Scene& scene, float dt) override
	{
		for (const auto& entity : Entities)
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();
			for (auto& script : scriptComponent.m_scripts)
			{
				script->OnUpdate(dt);
			}
		}
	}
};

} // namespace ecs
