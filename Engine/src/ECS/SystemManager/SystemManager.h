#pragma once

#include <memory>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../ComponentManager/ComponentManager.h"
#include "../EntityManager/EntityManager.h"
#include "../System/System.h"
#include "../TypeId/TypeId.h"

namespace ecs
{
using SystemId = TypeId;

class SystemManager final
{
public:
	template <typename _TSystem, typename... _TArgs>
	SystemManager& RegisterSystem(_TArgs&&... args)
	{
		m_currentSystemId = TypeIdOf<_TSystem>();

		assert(!m_systems.contains(m_currentSystemId)
			&& "Registering system more than once.");

		auto system = std::make_shared<_TSystem>(std::forward<_TArgs>(args)...);
		m_systems[m_currentSystemId] = system;
		m_signatures[m_currentSystemId] = Signature{};
		m_readDependencies[m_currentSystemId] = Signature{};

		return *this;
	}

	template <typename... _TComponents>
	SystemManager& WithRead()
	{
		assert(m_currentSystemId != InvalidEntity
			&& "No system selected for dependency registration.");

		(AddReadDependency<_TComponents>(), ...);

		return *this;
	}

	template <typename _TComponent>
	SystemManager& WithWrite()
	{
		assert(m_currentSystemId != InvalidEntity
			&& "No system selected for dependency registration.");

		assert(!m_writeDependencies.contains(m_currentSystemId)
			&& "System can only have one write dependency.");

		ComponentType componentType = TypeIdOf<_TComponent>();

		m_signatures[m_currentSystemId].set(componentType);
		m_writeDependencies[m_currentSystemId] = componentType;

		return *this;
	}

	template <typename _TSystem>
	_TSystem& GetSystem()
	{
		SystemId systemId = TypeIdOf<_TSystem>();
		assert(m_systems.contains(systemId)
			&& "Cannot get system: not registered.");

		return *static_cast<_TSystem*>(m_systems.at(systemId).get());
	}

	void OnEntitySignatureChanged(Entity entity, Signature entitySignature, Scene* scene)
	{
		auto wrapper = System::WrappedEntity{ scene, entity, entitySignature };

		for (auto const& [id, system] : m_systems)
		{
			Signature const& systemSignature = m_signatures.at(id);

			if ((entitySignature & systemSignature) == systemSignature)
			{
				if (std::ranges::find(system->Entities, wrapper) == system->Entities.end())
				{
					system->Entities.push_back(wrapper);
				}
			}
			else
			{
				std::erase(system->Entities, wrapper);
			}
		}
	}

	void BuildExecutionGraph()
	{
		std::unordered_map<SystemId, int> inDegree;
		std::unordered_map<SystemId, std::vector<SystemId>> adjList;

		for (auto const& [writerId, writtenComponent] : m_writeDependencies)
		{
			for (auto const& [readerId, readSignature] : m_readDependencies)
			{
				if (readerId != writerId && readSignature.test(writtenComponent))
				{
					adjList[writerId].push_back(readerId);
					inDegree[readerId]++;
				}
			}
		}

		std::queue<SystemId> q;
		for (auto const& [id, system] : m_systems)
		{
			if (!inDegree.contains(id))
			{
				q.push(id);
			}
		}

		m_executionStages.clear();
		while (!q.empty())
		{
			size_t stageSize = q.size();
			std::vector<SystemId> currentStage;
			for (size_t i = 0; i < stageSize; ++i)
			{
				SystemId u = q.front();
				q.pop();
				currentStage.push_back(u);

				if (adjList.contains(u))
				{
					for (SystemId v : adjList.at(u))
					{
						inDegree[v]--;
						if (inDegree[v] == 0)
						{
							q.push(v);
						}
					}
				}
			}
			m_executionStages.push_back(currentStage);
		}
	}

	void Execute(Scene& scene, float dt)
	{
		for (auto const& stage : m_executionStages)
		{
			std::vector<std::jthread> worker_threads;
			worker_threads.reserve(stage.size());

			for (SystemId id : stage)
			{
				worker_threads.emplace_back([this, id, &scene, dt]() {
					m_systems.at(id)->Update(scene, dt);
				});
			}
		}
	}

private:
	template <typename _TComponent>
	void AddReadDependency()
	{
		ComponentType componentType = TypeIdOf<_TComponent>();

		m_signatures[m_currentSystemId].set(componentType);
		m_readDependencies[m_currentSystemId].set(componentType);
	}

private:
	SystemId m_currentSystemId = InvalidEntity;

	std::unordered_map<SystemId, std::shared_ptr<System>> m_systems;
	std::unordered_map<SystemId, Signature> m_signatures;
	std::unordered_map<SystemId, Signature> m_readDependencies;
	std::unordered_map<SystemId, ComponentType> m_writeDependencies;

	std::vector<std::vector<SystemId>> m_executionStages;
};

} // namespace ecs
