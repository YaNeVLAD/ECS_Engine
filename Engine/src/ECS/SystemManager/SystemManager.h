#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../ComponentManager/ComponentManager.h"
#include "../EntityManager/EntityManager.h"
#include "../System/System.h"
#include "../TypeIndex/TypeIndex.h"

namespace Engine::ecs
{
using SystemId = TypeIndexType;

class SystemManager final
{
public:
	class SystemConfiguration
	{
	public:
		SystemConfiguration(SystemManager& manager, SystemId id)
			: m_id(id)
			, m_manager(manager)
		{
		}

		template <typename... _TComponents>
		SystemConfiguration& WithRead()
		{
			m_manager.AddReadDependencies<_TComponents...>(m_id);

			return *this;
		}

		template <typename _TComponent>
		SystemConfiguration& WithWrite()
		{
			m_manager.AddWriteDependency<_TComponent>(m_id);

			return *this;
		}

	private:
		SystemId m_id;
		SystemManager& m_manager;
	};

public:
	SystemManager();
	~SystemManager();

	template <typename _TSystem, typename... _TArgs>
	SystemConfiguration RegisterSystem(_TArgs&&... args);

	template <typename _TSystem>
	bool IsSystemRegistered();

	template <typename _TSystem>
	_TSystem& GetSystem();

	template <typename _TSystem>
	_TSystem const& GetSystem() const;

	void OnEntitySignatureChanged(Entity entity, Signature entitySignature, Scene* scene);

	void BuildExecutionGraph();

	void Execute(Scene& scene, float dt);

private:
	template <typename... _TComponents>
	void AddReadDependencies(SystemId systemId);

	template <typename _TComponent>
	void AddWriteDependency(SystemId systemId);

	void WorkerLoop();

private:
	SystemId m_currentSystemId = InvalidEntity;

	std::unordered_map<SystemId, std::unique_ptr<System>> m_systems;
	std::unordered_map<SystemId, Signature> m_signatures;
	std::unordered_map<SystemId, Signature> m_readDependencies;
	std::unordered_map<SystemId, ComponentType> m_writeDependencies;

	std::vector<std::vector<SystemId>> m_executionStages;

	std::unordered_map<Entity, size_t> m_entityToIndexMap;

	std::vector<std::jthread> m_workerThreads;
	std::queue<std::function<void()>> m_taskQueue;
	std::mutex m_queueMutex;
	std::condition_variable m_workerCondition;
	std::condition_variable m_mainCondition;
	std::atomic<size_t> m_tasksInProgress = 0;
	std::atomic<bool> m_stopThreads = false;
};

} // namespace Engine::ecs

#include "SystemManager.impl"
