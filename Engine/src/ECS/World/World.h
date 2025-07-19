#pragma once

#include <memory>
#include <vector>

#include "../ComponentManager/ComponentManager.h"
#include "../Entity/Entity.h"
#include "../Entity/Signature.h"
#include "../EntityManager/EntityManager.h"
#include "../SystemManager/SystemManager.h"

namespace ecs
{

class World
{
public:
	World()
		: m_componentManager(std::make_unique<ComponentManager>())
		, m_entityManager(std::make_unique<EntityManager>())
		, m_systemManager(std::make_unique<SystemManager>())
	{
	}

	Entity CreateEntity()
	{
		return m_entityManager->CreateEntity();
	}

	void DestoryEntity(Entity entity)
	{
		m_entitiesToDestroy.push_back(entity);
	}

	template <typename _TComponent>
	void RegisterComponent()
	{
		m_componentManager->RegisterComponent<_TComponent>();
	}

	template <typename _TComponent>
	void AddComponent(Entity entity, _TComponent const& component)
	{
		m_componentManager->AddComponent(entity, component);

		auto& signature = m_entityManager->GetSignature(entity);
		signature.set(TypeIdOf<_TComponent>());
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->OnEntitySignatureChanged(entity, signature);
	}

	template <typename _TComponent>
	void RemoveComponent(Entity entity)
	{
		m_componentManager->RemoveComponent<_TComponent>(entity);

		auto& signature = m_entityManager->GetSignature(entity);
		signature.set(TypeIdOf<_TComponent>());
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->OnEntitySignatureChanged(entity, signature);
	}

	template <typename _TComponent>
	_TComponent& GetComponent(Entity entity)
	{
		return m_componentManager->GetComponent<_TComponent>(entity);
	}

	template <typename _TComponent>
	bool HasComponent(Entity entity) const
	{
		return m_componentManager->HasComponent<_TComponent>(entity);
	}

	template <typename _TSystem, typename... _TArgs>
	SystemManager& RegisterSystem(_TArgs&&... args)
	{
		return m_systemManager->RegisterSystem<_TSystem>(std::forward<_TArgs>(args)...);
	}

	template <typename _TSystem>
	_TSystem& GetSystem()
	{
		return m_systemManager->GetSystem<_TSystem>();
	}

	void BuildSystemGraph()
	{
		m_systemManager->BuildExecutionGraph();
	}

	void Update(float dt)
	{
		m_systemManager->Execute(*this, dt);
	}

	ComponentManager& GetComponentManager()
	{
		return *m_componentManager;
	}

	void ConfirmChanges()
	{
		for (Entity const& entity : m_entitiesToDestroy)
		{
			m_entityManager->DestroyEntity(entity);
			m_componentManager->OnEntityDestroyed(entity);
			m_systemManager->OnEntitySignatureChanged(entity, {});
		}

		m_entitiesToDestroy.clear();
	}

private:
	std::unique_ptr<ComponentManager> m_componentManager;
	std::unique_ptr<EntityManager> m_entityManager;
	std::unique_ptr<SystemManager> m_systemManager;

	std::vector<Entity> m_entitiesToDestroy;
};

} // namespace ecs
