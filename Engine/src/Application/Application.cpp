#include "Application.h"

#include <chrono>

namespace Engine
{

Application::Application()
{
	m_window = std::make_unique<render::Window>(render::WindowProps{ "My Engine App", 1920, 1080 });
}

void Application::Run()
{
	render::Renderer2D::Init();
	OnCreate();

	auto lastTime = std::chrono::high_resolution_clock::now();

	while (!m_window->ShouldClose() && m_running)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;

		OnUpdate(dt);

		m_window->OnUpdate();
	}

	OnDestroy();
	render::Renderer2D::Shutdown();
}

void Application::Close()
{
	m_running = false;
}

render::Window& Application::Window()
{
	return *m_window;
}

ecs::Scene& Application::Scene()
{
	return m_scene;
}

} // namespace Engine
