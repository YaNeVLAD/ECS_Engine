#pragma once

#include <ranges>
#include <vector>

#include "../ComponentManager/ComponentManager.h"
#include "IView.h"
#include "Iterator/ViewIterator.h"

namespace ecs
{
template <typename... _TComponents>
class View final : public IView
{
private:
	class Iterable
	{
	public:
		Iterable(ComponentManager& manager, std::vector<Entity>& entities)
			: m_manager(manager)
			, m_entities(entities)
		{
		}
		auto begin() { return ViewIterator<_TComponents...>(m_manager, m_entities.begin()); }
		auto end() { return ViewIterator<_TComponents...>(m_manager, m_entities.end()); }

	private:
		ComponentManager& m_manager;
		std::vector<Entity>& m_entities;
	};

public:
	View(ComponentManager& manager, Signature signature)
		: m_manager(manager)
		, m_signature(signature)
	{
	}

	Iterable Each()
	{
		return Iterable(m_manager, m_entities);
	}

	void AddEntity(Entity entity)
	{
		m_entities.push_back(entity);
	}

	void OnEntityDestroyed(Entity entity) override
	{
		std::erase(m_entities, entity);
	}

	void OnEntitySignatureChanged(Entity entity, Signature entitySignature) override
	{
		if ((entitySignature & m_signature) == m_signature)
		{
			if (std::ranges::find(m_entities, entity) == m_entities.end())
			{
				m_entities.push_back(entity);
			}
		}
		else
		{
			std::erase(m_entities, entity);
		}
	}

private:
	ComponentManager& m_manager;
	Signature m_signature;
	std::vector<Entity> m_entities;
};
} // namespace ecs
