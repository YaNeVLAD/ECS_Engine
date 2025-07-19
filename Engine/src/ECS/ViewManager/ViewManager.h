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
	CreateView(ComponentManager& componentManager, EntityManager const& entityManager)
	{
		Signature signature = CreateSignature<_TComponents...>();

		if (m_views.contains(signature))
		{
			return std::dynamic_pointer_cast<View<_TComponents...>>(m_views.at(signature));
		}

		auto view = std::make_shared<View<_TComponents...>>(componentManager, signature);

		for (Entity entity : entityManager.GetActiveEntities())
		{
			if ((entityManager.GetSignature(entity) & signature) == signature)
			{
				view->AddEntity(entity);
			}
		}

		m_views[signature] = view;
		return view;
	}

	void OnEntityDestroyed(Entity entity)
	{
		for (auto const& [_, view] : m_views)
		{
			view->OnEntityDestroyed(entity);
		}
	}

	void OnEntitySignatureChanged(Entity entity, Signature signature)
	{
		for (auto const& [_, view] : m_views)
		{
			view->OnEntitySignatureChanged(entity, signature);
		}
	}

private:
	template <typename... _TComponents>
	Signature CreateSignature()
	{
		Signature signature;
		(signature.set(TypeIdOf<_TComponents>()), ...);
		return signature;
	}

	std::unordered_map<Signature, std::shared_ptr<IView>> m_views;
};

} // namespace ecs
