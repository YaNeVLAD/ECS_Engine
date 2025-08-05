#pragma once

#include "../Entity/Entity.h"
#include "../Entity/Signature.h"

namespace ecs
{

template <typename _TScene>
class EntityWrapper
{
public:
	EntityWrapper(_TScene* scene, Entity id, Signature signature)
		: m_scene(scene)
		, m_id(id)
		, m_signature(signature)
	{
	}

	Entity GetEntity() const
	{
		return m_id;
	}

	Signature GetSignature() const
	{
		return m_signature;
	}

	template <typename _TComponent>
	void AddComponent(_TComponent const& component)
	{
		m_scene->AddComponent<_TComponent>(m_id, component);
	}

	template <typename _TComponent, typename... _TArgs>
	void AddComponent(_TArgs&&... args)
	{
		m_scene->AddComponent<_TComponent>(
			m_id,
			_TComponent{ std::forward<_TArgs>(args)... });
	}

	template <typename _TComponent>
	_TComponent& GetComponent()
	{
		return m_scene->GetComponent<_TComponent>(m_id);
	}

	template <typename _TComponent>
	_TComponent const& GetComponent() const
	{
		return m_scene->GetComponent<_TComponent>(m_id);
	}

	template <typename _TComponent>
	bool HasComponent()
	{
		return m_scene->HasComponent<_TComponent>(m_id);
	}

	template <typename _TComponent>
	void RemoveComponent()
	{
		m_scene->RemoveComponent<_TComponent>(m_id);
	}

	void Destroy()
	{
		m_scene->DestoryEntity(m_id);
	}

	auto operator<=>(EntityWrapper const& other) const
	{
		return m_id <=> other.m_id;
	}

	bool operator==(EntityWrapper const& other) const
	{
		return m_id == other.m_id;
	}

	bool operator==(Entity const& entity) const
	{
		return m_id == entity;
	}

private:
	_TScene* m_scene;
	Entity m_id;
	Signature m_signature;
};

} // namespace ecs
