#pragma once

#include "../../ECS/Entity/Entity.h"

namespace ecs
{

class Scene;

class ScriptableEntity
{
public:
	virtual ~ScriptableEntity() = default;

	virtual void OnCreate() {}
	virtual void OnUpdate(float dt) {}
	virtual void OnDestroy() {}

protected:
	template <typename _T>
	_T& GetComponent()
	{
		return m_scene->GetComponent<_T>(m_entity);
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

} // namespace ecs
