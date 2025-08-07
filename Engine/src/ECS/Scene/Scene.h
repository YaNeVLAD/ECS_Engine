#pragma once

#include <memory>
#include <vector>

#include "../ComponentManager/ComponentManager.h"
#include "../Entity/Entity.h"
#include "../EntityManager/EntityManager.h"
#include "../SystemManager/SystemManager.h"
#include "../ViewManager/ViewManager.h"

namespace ecs
{

class Scene
{
public:
	Scene()
		: m_componentManager(std::make_unique<ComponentManager>())
		, m_entityManager(std::make_unique<EntityManager>())
		, m_systemManager(std::make_unique<SystemManager>())
		, m_viewManager(std::make_unique<ViewManager>())
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

	template <typename... _TComponents>
	void RegisterComponents()
	{
		(m_componentManager->RegisterComponent<_TComponents>(), ...);
	}

	template <typename _T>
	bool IsRegistered()
	{
		if constexpr (std::is_base_of_v<System, _T>)
		{
			return m_systemManager->IsSystemRegistered<_T>();
		}

		return m_componentManager->IsComponentRegistered<_T>();
	}

	template <typename _TComponent>
	void AddComponent(Entity entity, _TComponent const& component)
	{
		AddComponentImpl(entity, component);
	}

	template <typename _TComponent, typename... _TArgs>
	void AddComponent(Entity entity, _TArgs&&... args)
	{
		auto component = _TComponent{ std::forward<_TArgs>(args)... };
		AddComponentImpl(entity, std::move(component));
	}

	template <typename _TComponent>
	void RemoveComponent(Entity entity)
	{
		m_componentManager->RemoveComponent<_TComponent>(entity);

		auto& signature = m_entityManager->GetSignature(entity);
		signature.set(TypeIdOf<_TComponent>());
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->OnEntitySignatureChanged(entity, signature, this);
	}

	template <typename _TComponent>
	_TComponent& GetComponent(Entity entity)
	{
		return m_componentManager->GetComponent<_TComponent>(entity);
	}

	template <typename _TComponent>
	_TComponent const& GetComponent(Entity entity) const
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
	_TSystem& GetSystem() const
	{
		return m_systemManager->GetSystem<_TSystem>();
	}

	void BuildSystemGraph()
	{
		m_systemManager->BuildExecutionGraph();
	}

	void TakeStep(float dt)
	{
		m_systemManager->Execute(*this, dt);
	}

	ComponentManager& GetComponentManager() const
	{
		return *m_componentManager;
	}

	void ConfirmChanges()
	{
		for (Entity const& entity : m_entitiesToDestroy)
		{
			m_entityManager->DestroyEntity(entity);
			m_componentManager->OnEntityDestroyed(entity);
			m_systemManager->OnEntitySignatureChanged(entity, {}, this);
			m_viewManager->OnEntityDestroyed(entity);
		}

		m_entitiesToDestroy.clear();
	}

	template <typename... _TComponents>
	auto CreateView()
	{
		return m_viewManager->CreateView<_TComponents...>(*m_componentManager, *m_entityManager);
	}

private:
	template <typename _TComponent>
	void AddComponentImpl(Entity entity, _TComponent component)
	{
		m_componentManager->AddComponent(entity, component);

		auto& signature = m_entityManager->GetSignature(entity);
		signature.set(TypeIdOf<_TComponent>());
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->OnEntitySignatureChanged(entity, signature, this);
		m_viewManager->OnEntitySignatureChanged(entity, signature);
	}

private:
	std::unique_ptr<ComponentManager> m_componentManager;
	std::unique_ptr<EntityManager> m_entityManager;
	std::unique_ptr<SystemManager> m_systemManager;
	std::unique_ptr<ViewManager> m_viewManager;

	std::vector<Entity> m_entitiesToDestroy;
};

} // namespace ecs
