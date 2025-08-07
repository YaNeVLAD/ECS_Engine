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
	SystemManager& RegisterSystem(_TArgs&&... args);

	template <typename _TSystem>
	bool IsSystemRegistered();

	template <typename... _TComponents>
	SystemManager& WithRead();

	template <typename _TComponent>
	SystemManager& WithWrite();

	template <typename _TSystem>
	_TSystem& GetSystem() const;

	void OnEntitySignatureChanged(Entity entity, Signature entitySignature, Scene* scene);

	void BuildExecutionGraph();

	void Execute(Scene& scene, float dt);

private:
	template <typename _TComponent>
	void AddReadDependency();

private:
	SystemId m_currentSystemId = InvalidEntity;

	std::unordered_map<SystemId, std::shared_ptr<System>> m_systems;
	std::unordered_map<SystemId, Signature> m_signatures;
	std::unordered_map<SystemId, Signature> m_readDependencies;
	std::unordered_map<SystemId, ComponentType> m_writeDependencies;

	std::vector<std::vector<SystemId>> m_executionStages;
};

} // namespace ecs

#include "SystemManager.impl"
