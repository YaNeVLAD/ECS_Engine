#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace ecs
{
/**
 * @brief Базовое событие.
 * @brief Все пользовательские события
 * @brief должны наследоваться от этого класса.
 */
struct event
{
	virtual ~event() = default;
};

/**
 * @brief Класс шины событий
 */
class event_bus
{
	using event_id = std::type_index;
	using handler = std::function<void(const event&)>;
	using subscribers = std::unordered_map<event_id, std::vector<handler>>;

	subscribers m_subscribers;
	std::mutex m_mutex;

public:
	/**
	 * @brief Добавляет функцию-обработчик для события.
	 * @brief Функция-обработчик будет вызвана при отправке указанного события.
	 * @see ecs::event_bus::publish()
	 */
	template <typename _TEvent>
	void subscribe(std::function<void(const _TEvent&)> callback)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto ID = id_of<_TEvent>();
		m_subscribers[ID].push_back(
			[callback](const event& e) { callback(static_cast<const _TEvent&>(e)); });
	}

	/**
	 * @brief Отправляет переданное событие всем его обработчикам.
	 * @see ecs::event_bus::publish()
	 */
	template <typename _TEvent>
	void publish(const _TEvent& event)
	{
		std::vector<handler> handlers;
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			auto ID = id_of(event);
			auto it = m_subscribers.find(ID);
			if (it != m_subscribers.end())
			{
				handlers = it->second;
			}
		}

		std::vector<std::thread> threads;
		for (const auto& handler : handlers)
		{
			threads.emplace_back([handler, &event]() { handler(event); });
		}

		for (auto& thread : threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
	}

private:
	template <typename _TEvent>
	std::type_index id_of(const _TEvent&)
	{
		return std::type_index(typeid(_TEvent));
	}
};
} // namespace ecs
