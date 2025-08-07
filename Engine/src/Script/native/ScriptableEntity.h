#pragma once

#include "../../ECS/Scene/Scene.h"

namespace ecs::scripts
{

class ScriptableEntity
{
public:
	virtual ~ScriptableEntity() = default;

	virtual void OnCreate() {}
	virtual void OnUpdate(float) {}
	virtual void OnDestroy() {}

protected:
	template <typename _TComponent>
	_TComponent& GetComponent()
	{
		return m_scene->GetComponent<_TComponent>(m_entity);
	}

	ecs::Scene& Scene()
	{
		return *m_scene;
	}

	ecs::Entity Entity() const
	{
		return m_entity;
	}

private:
	ecs::Entity m_entity = InvalidEntity;
	ecs::Scene* m_scene = nullptr;

	friend class ScriptComponent;
};

} // namespace ecs::scripts
