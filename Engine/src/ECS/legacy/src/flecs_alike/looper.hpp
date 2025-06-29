#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

#include "system_manager.hpp"

namespace ecs
{
class looper
{
	using clock = std::chrono::high_resolution_clock;
	using duration = std::chrono::duration<float>;
	using renderer = std::function<bool(ecs::context&)>;

	system_manager& m_manager;
	renderer m_renderer = nullptr;

public:
	looper(system_manager& manager)
		: m_manager(manager)
	{
	}

	void frame(float delta_time)
	{
		m_manager.update(delta_time);
	}

	void render_callback(renderer __fn) { m_renderer = std::move(__fn); }

	void run(std::optional<unsigned int> targetFPS = std::nullopt)
	{
		auto frame_duration = targetFPS.has_value()
			? duration(1.0f / targetFPS.value())
			: duration(0);

		while (true)
		{
			auto start = clock::now();

			float delta_time = std::chrono::duration_cast<duration>(clock::now() - start).count();

			frame(delta_time);

			if (m_renderer != nullptr && !m_renderer(m_manager.get_context()))
			{
				break;
			}

			if (targetFPS.has_value())
			{
				auto elapsed = clock::now() - start;
				auto sleep_duration = frame_duration - elapsed;

				if (sleep_duration > std::chrono::milliseconds(0))
				{
					std::this_thread::sleep_for(sleep_duration);
				}
			}
		}
	}
};
} // namespace ecs
