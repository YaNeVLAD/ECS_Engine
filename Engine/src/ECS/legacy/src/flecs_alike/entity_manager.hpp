#pragma once

#include <algorithm>
#include <optional>
#include <vector>

#include "entity.hpp"
#include "iterator.hpp"

namespace ecs
{
/**
 * @brief Класс для работы с коллекцией сущностей.
 */
class entity_manager
{
public:
	/**
	 * @brief Метод для установки диапазона сущностей.
	 * @brief Можно указать новый диапазон или уже существующий.
	 */
	template <typename _TScope>
	entity_manager& at_scope()
	{
		m_currentScope = typeid(_TScope);
		return *this;
	}

	/**
	 * @brief Возвращает все сущности в выбранном диапазоне.
	 * @see ecs::entity_manager::at_scope()
	 */
	std::vector<entity*>& all()
	{
		return (m_currentScope)
			? m_scopes[*m_currentScope]
			: m_scopes[m_defaultScope];
	}

	/**
	 * @brief Создаёт новую сущность в выбранном диапазоне.
	 * @brief Если диапазон не указан - создаёт сущность в стандартном диапазоне.
	 * @see ecs::entity_manager::at_scope()
	 */
	entity& create()
	{
		auto& entity = create_internal();
		(m_currentScope)
			? m_scopes[*m_currentScope].push_back(&entity)
			: m_scopes[m_defaultScope].push_back(&entity);
		update_all();
		m_currentScope.reset();
		return entity;
	}

	/**
	 * @brief Удаляет все невалидные сущности из коллекции.
	 */
	void invalidate()
	{
		for (auto& [_, entities] : m_scopes)
		{
			entities.erase(
				std::remove_if(entities.begin(), entities.end(),
					[](entity* e) { return !e->is_valid(); }),
				entities.end());
		}
		update_all();
		m_currentScope.reset();
	}

	/**
	 * @brief Метод возвращает пару итераторов, указывающих на начало и конец коллекции сущностей.
	 * @brief Метод позволяет перебрать всю коллекцию сущностей, независимо от выбранного диапазона.
	 */
	std::pair<iterator<entity>, iterator<entity>> all_entities()
	{
		return { iterator(m_allEntities), iterator(m_allEntities, m_allEntities.size()) };
	}

	/**
	 * @brief Очищает коллекцию сущностей и переводит класс в начальное состояние.
	 * @brief Работа метода не зависит от выбранного диапазона.
	 */
	void reset()
	{
		m_entities.clear();
		m_scopes.clear();
		m_scopes[m_defaultScope] = {};
		m_allEntities.clear();
		m_currentScope.reset();
		m_nextID = 0;
	}

private:
	struct initial
	{
	};

	size_t m_nextID = 0;

	const std::type_index m_defaultScope = typeid(initial);
	std::optional<std::type_index> m_currentScope;
	std::vector<std::unique_ptr<entity>> m_entities;
	std::vector<entity*> m_allEntities;
	std::unordered_map<std::type_index, std::vector<entity*>> m_scopes = { { m_defaultScope, {} } };

	entity& create_internal()
	{
		m_entities.emplace_back(std::make_unique<entity>(m_nextID++));
		return *m_entities.back();
	}

	void update_all()
	{
		m_allEntities.clear();
		for (const auto& [_, entities] : m_scopes)
		{
			m_allEntities.insert(m_allEntities.end(), entities.begin(), entities.end());
		}
	}
};
} // namespace ecs
