#pragma once

#include "entity_manager.hpp"
#include "event_bus.hpp"

namespace ecs
{
/**
 * @brief Класс контекста.
 * @brief Предоставляет некоторую служебную информацию
 * @brief внутри системы
 */
class context
{
public:
	context(ecs::entity_manager& manager, ecs::event_bus& bus)
		: m_manager(manager)
		, m_event_bus(bus)
	{
	}

	float delta_time = 0;
	size_t entity_id = 0;

	ecs::entity_manager& entity() { return m_manager; }
	ecs::event_bus& event_bus() { return m_event_bus; }

private:
	ecs::entity_manager& m_manager;
	ecs::event_bus& m_event_bus;
};
} // namespace ecs
