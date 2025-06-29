#pragma once

#include <string>
#include <typeindex>
#include <vector>

#include "context.hpp"

namespace ecs
{
/**
 * @brief Класс системы
 */
class system_impl
{
public:
	using callback_type = std::function<void(context&, std::vector<void*>&)>;

	system_impl(std::string name)
		: m_name(std::move(name))
	{
	}

	const std::vector<std::type_index>& filters() const { return m_filters; }

	void callback(callback_type __fn) { m_callback = std::move(__fn); }
	const callback_type& callback() const { return m_callback; }

	void add_filter(std::type_index filter) { m_filters.push_back(filter); }

private:
	std::string m_name;
	std::vector<std::type_index> m_filters;
	callback_type m_callback;
};
} // namespace ecs
