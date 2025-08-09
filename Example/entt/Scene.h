#pragma once

#include <entt/entt.hpp>

using Entity = entt::entity;

class Scene;

class System
{
public:
	virtual ~System() = default;
	virtual void Update(Scene& scene, float dt) = 0;
};

class Scene
{
public:
	Scene() = default;

	// ��������� ����������� � �����������, �.�. ������ �������
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	Entity CreateEntity()
	{
		return m_registry.create();
	}

	void DestoryEntity(Entity entity)
	{
		m_entitiesToDestroy.push_back(entity);
	}

	// � entt �� ��������� ����� ����������� �����������.
	// ��� ������� ��������� ��� ������������� API, �� ��� ������ �� ������.
	template <typename _TComponent>
	void RegisterComponent()
	{
		// no-op
	}

	template <typename... _TComponents>
	void RegisterComponents()
	{
		// no-op
	}

	template <typename _T>
	bool IsRegistered()
	{
		return true;
	}

	template <typename _TComponent>
	void AddComponent(Entity entity, _TComponent const& component)
	{
		// emplace_or_replace ������� ���������, ���� ��� ���, ��� ������� ������������.
		m_registry.emplace_or_replace<_TComponent>(entity, component);
	}

	template <typename _TComponent, typename... _TArgs>
	void AddComponent(Entity entity, _TArgs&&... args)
	{
		m_registry.emplace_or_replace<_TComponent>(entity, std::forward<_TArgs>(args)...);
	}

	template <typename _TComponent>
	void RemoveComponent(Entity entity)
	{
		// remove ��������� ������ �� ������, ���� ���������� ���
		m_registry.remove<_TComponent>(entity);
	}

	template <typename _TComponent>
	_TComponent& GetComponent(Entity entity)
	{
		return m_registry.get<_TComponent>(entity);
	}

	template <typename _TComponent>
	_TComponent const& GetComponent(Entity entity) const
	{
		return m_registry.get<_TComponent>(entity);
	}

	template <typename _TComponent>
	bool HasComponent(Entity entity) const
	{
		// any_of - ����������� ������ �������� ������� ������ ��� ���������� �����������
		return m_registry.any_of<_TComponent>(entity);
	}

	template <typename _TSystem, typename... _TArgs>
	_TSystem& RegisterSystem(_TArgs&&... args)
	{
		auto system = std::make_shared<_TSystem>(std::forward<_TArgs>(args)...);

		m_systemExecutionOrder.push_back(system);

		return *system;
	}

	template <typename _TSystem>
	_TSystem& GetSystem()
	{
		return static_cast<_TSystem&>(*m_systemExecutionOrder.back());
	}

	// � entt ��� ����������� ����� ������������ ������.
	// ������ ������� ������ ����������� � ������� �� �����������.
	// ��������� ����� ��� no-op ��� ������������� API.
	void BuildSystemGraph()
	{
		// no-op
	}

	void Frame(float dt)
	{
		// ������ ��������� ��� ������� � ������� �� �����������
		for (auto const& system : m_systemExecutionOrder)
		{
			system->Update(*this, dt);
		}
	}

	// ���������� ��� registry, ��� ��� ��� �������� ������� ���������� � entt.
	entt::registry& GetRegistry()
	{
		return m_registry;
	}

	// ����������� ������ ��� ����������� ����
	entt::registry const& GetRegistry() const
	{
		return m_registry;
	}

	void ConfirmChanges()
	{
		// entt::registry::destroy ����� ��������� �������� ����������
		m_registry.destroy(m_entitiesToDestroy.begin(), m_entitiesToDestroy.end());
		m_entitiesToDestroy.clear();
	}

	template <typename... _TComponents>
	auto CreateView()
	{
		return m_registry.view<_TComponents...>();
	}

private:
	entt::registry m_registry;
	std::vector<Entity> m_entitiesToDestroy;

	std::vector<std::shared_ptr<System>> m_systemExecutionOrder;
};
