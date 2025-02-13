#pragma once

#include <functional>
#include <string>
#include <vector>

#include "context.hpp"
#include "entity_manager.hpp"
#include "system.hpp"

namespace ecs
{
namespace detail
{
template <typename _TFn, typename... _TComponents, std::size_t... Is>
void invoke_with_components_impl(_TFn&& __fn, context& ctx, std::vector<void*>& components, std::index_sequence<Is...>)
{
	std::invoke(std::forward<_TFn>(__fn), ctx, *static_cast<_TComponents*>(components[Is])...);
}

template <typename... _TComponents, typename _TFn>
void invoke_with_components(_TFn&& __fn, context& ctx, std::vector<void*>& components)
{
	invoke_with_components_impl<_TFn, _TComponents...>(std::forward<_TFn>(__fn), ctx, components, std::make_index_sequence<sizeof...(_TComponents)>());
}
} // namespace detail

class system_storage
{
public:
	virtual ~system_storage() = default;
	virtual std::vector<std::unique_ptr<system_impl>>& get_systems() = 0;
};

/**
 * @brief Класс для конструирования системы
 */
template <typename... _TComponents>
class system_builder
{
public:
	system_builder(system_storage& manager, std::string name)
		: m_manager(manager)
		, m_system{ std::move(name) }
	{
		(m_system.add_filter(typeid(_TComponents)), ...);
	}

	/**
	 * @brief Добавляет фильтр к конструируемой системе.
	 */
	template <typename _TComponent>
	system_builder& with()
	{
		m_system.add_filter(typeid(_TComponent));
		return *this;
	}

	/**
	 * @brief Завершает создание системы и устанавливает функцию-обработчик.
	 * @brief Функция-обработчик должна принимать аргументы в том же порядке,
	 * @brief в каком они указаны в системе.
	 * @see ecs::system_manager::system()
	 */
	template <typename _TFn>
	system_storage& each(_TFn&& __fn)
	{
		return set_each_callback([callback = std::forward<_TFn>(__fn)](context& ctx, std::vector<void*>& components) {
			if constexpr (sizeof...(_TComponents) > 0)
				detail::invoke_with_components<_TComponents...>(
					[&callback](context& _context, _TComponents&... _components) {
						(void)_context;
						std::invoke(callback, _components...);
					},
					ctx, components);
			else
				std::invoke(callback);
		});
	}

	/**
	 * @brief Завершает создание системы и устанавливает метод-обработчик.
	 * @brief Метод-обработчик должен принимать первым аргументом контекст.
	 * @brief Метод-обработчик должен принимать аргументы в том же порядке,
	 * @brief в каком они указаны в системе.
	 * @brief Также требует ссылку на объект класса.
	 * @see ecs::system_manager::system()
	 */
	template <typename _TMethod, class _TClass>
	system_storage& each(_TMethod __fn, _TClass& instance)
	{
		return set_each_callback([callback = std::forward<_TMethod>(__fn), &instance](context& ctx, std::vector<void*>& components) {
			if constexpr (sizeof...(_TComponents) > 0)
				detail::invoke_with_components<_TComponents...>(
					[&callback, &instance](context& _context, _TComponents&... _components) {
						(instance.*callback)(_components...);
					},
					ctx, components);
			else
				std::invoke(callback);
		});
	}

	/**
	 * @brief Завершает создание системы и устанавливает метод-обработчик.
	 * @brief Метод-обработчик должен принимать первым аргументом экземпляр контекста.
	 * @brief Метод-обработчик должен принимать аргументы в том же порядке,
	 * @brief в каком они указаны в системе.
	 * @brief Также требует ссылку на объект класса.
	 * @see ecs::system_manager::system()
	 */
	template <typename _TMethod, class _TClass>
	system_storage& each(_TMethod __fn, _TClass& instance, bool context_needed)
	{
		return set_each_callback([callback = std::forward<_TMethod>(__fn), &instance](context& ctx, std::vector<void*>& components) {
			if constexpr (sizeof...(_TComponents) > 0)
				detail::invoke_with_components<_TComponents...>(
					[&callback, &instance](context& _context, _TComponents&... _components) {
						(instance.*callback)(_context, _components...);
					},
					ctx, components);
			else
				std::invoke(callback);
		});
	}

	/**
	 * @brief Завершает создание системы и устанавливает функцию-обработчик.
	 * @brief Функция-обработчик должна принимать первым аргументом контекст.
	 * @brief Функция-обработчик должна принимать аргументы в том же порядке,
	 * @brief в каком они указаны в системе.
	 * @see ecs::system_manager::system()
	 */
	template <typename _TFn>
	system_storage& each(_TFn&& __fn, bool context_needed)
	{
		return set_each_callback([callback = std::forward<_TFn>(__fn)](context& ctx, std::vector<void*>& components) {
			if constexpr (sizeof...(_TComponents) > 0)
				detail::invoke_with_components<_TComponents...>(
					[&callback](context& _context, _TComponents&... _components) {
						std::invoke(callback, _context, _components...);
					},
					ctx, components);
			else
				std::invoke(callback, ctx);
		});
	}

private:
	system_impl m_system;
	system_storage& m_manager;

	template <typename _TFn>
	system_storage& set_each_callback(_TFn&& _function)
	{
		m_system.callback(std::forward<_TFn>(_function));
		m_manager.get_systems().push_back(std::make_unique<system_impl>(std::move(m_system)));
		return m_manager;
	}
};

/**
 * @brief Класс для работы с коллекцией систем
 */
class system_manager : private system_storage
{
public:
	system_manager(entity_manager& em)
		: m_entityManager(em)
		, m_context(em, m_event_bus)
	{
	}

	/**
	 * @brief Возвращает класс для конструирования системы.
	 * @brief Важен порядок в котором передаются компоненты.
	 * @see ecs::system_builder::each()
	 */
	template <typename... _TComponents>
	auto system(std::string name)
	{
		return system_builder<_TComponents...>(*this, std::move(name));
	}

	/**
	 * @brief Возвращает шину событий
	 */
	ecs::event_bus& event_bus() { return m_event_bus; }

	/**
	 * @brief Возвращает контекст
	 */
	ecs::context& get_context() { return m_context; }

	/**
	 * @brief Вызывает функцию-обработчик для каждой системы
	 */
	void update(float delta_time)
	{
		auto [begin, end] = m_entityManager.all_entities();
		if (begin == end)
		{
			return;
		}
		for (auto& entity = begin; entity != end; ++entity)
		{
			if (!entity->is_valid())
				continue;

			for (const auto& system : m_systems)
			{
				std::vector<void*> components;
				components.reserve(system->filters().size());
				bool hasAll = true;
				for (const auto& filter : system->filters())
				{
					if (entity->has(filter))
					{
						components.push_back(entity->get(filter));
					}
					else
					{
						hasAll = false;
						break;
					}
				}
				if (hasAll && components.size() == system->filters().size())
				{
					m_context.entity_id = entity->ID();
					m_context.delta_time = delta_time;
					system->callback()(m_context, components);
				}
			}
		}
	}

private:
	entity_manager& m_entityManager;
	std::vector<std::unique_ptr<system_impl>> m_systems;
	context m_context;
	ecs::event_bus m_event_bus;

	std::vector<std::unique_ptr<system_impl>>& get_systems() override
	{
		return m_systems;
	}
};
} // namespace ecs
