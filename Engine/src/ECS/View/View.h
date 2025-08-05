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
public:
	using Iterator = ViewIterator<false, _TComponents...>;
	using ConstIterator = ViewIterator<true, _TComponents...>;

	View(ComponentManager& manager, Signature signature)
		: m_manager(manager)
		, m_signature(signature)
	{
	}

	auto begin() { return Iterator(m_manager, m_entities.begin()); }
	auto end() { return Iterator(m_manager, m_entities.end()); }

	auto begin() const { return ConstIterator(m_manager, m_entities.cbegin()); }
	auto end() const { return ConstIterator(m_manager, m_entities.cend()); }

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
