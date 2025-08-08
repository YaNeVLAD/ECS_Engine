#pragma once

#include <memory>

#include "../EntityManager/EntityManager.h"
#include "../TypeId/TypeId.h"
#include "../View/View.h"

namespace ecs
{

class ViewManager final
{
public:
	template <typename... _TComponents>
	std::shared_ptr<View<_TComponents...>>
	CreateView(ComponentManager& componentManager, EntityManager const& entityManager);

	void OnEntityDestroyed(Entity entity);

	void OnEntitySignatureChanged(Entity entity, Signature signature);

private:
	template <typename... _TComponents>
	Signature CreateSignature();

	std::unordered_map<Signature, std::shared_ptr<IView>> m_views;
};

} // namespace ecs

#include "ViewManager.impl"
