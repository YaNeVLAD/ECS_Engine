#pragma once

#include <memory>
#include <vector>

#include "ScriptableEntity.h"

namespace Engine::scripts
{

class ScriptableEntity;

class ScriptComponent
{
public:
	~ScriptComponent()
	{
		for (auto& script : m_scripts)
		{
			script->OnDestroy();
		}
	}

	template <typename _TScript, typename... _TArgs>
	void Bind(ecs::Scene& scene, ecs::Entity entity, _TArgs&&... args)
	{
		auto script = std::make_shared<_TScript>(std::forward<_TArgs>(args)...);

		script->m_entity = entity;
		script->m_scene = &scene;

		m_scripts.push_back(std::move(script));
		m_scripts.back()->OnCreate();
	}

private:
	std::vector<std::shared_ptr<ScriptableEntity>> m_scripts;

	friend class ScriptingSystem;
};

template <typename _TScript, typename... _TArgs>
void Bind(ecs::Scene& scene, ecs::Entity entity, _TArgs&&... args)
{
	if (scene.IsRegistered<ScriptComponent>())
	{
		scene.GetComponent<ScriptComponent>(entity)
			.Bind<_TScript>(scene, entity, args...);
	}
}

} // namespace Engine::scripts
