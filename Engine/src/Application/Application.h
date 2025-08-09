#pragma once

#include <memory>

#include "../ECS/Scene/Scene.h"
#include "../Render/2D/Renderer2D.h"
#include "../Render/Common/Window/Window.h"

namespace Engine
{

class Application
{
public:
	Application();

	virtual ~Application() = default;

	void Run();

	void Close();

	virtual void OnCreate() = 0;
	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnDestroy() = 0;

protected:
	render::Window& Window();
	ecs::Scene& Scene();

private:
	bool m_running = true;

	ecs::Scene m_scene;

	std::unique_ptr<render::Window> m_window;
};

Application* CreateApplication();

} // namespace Engine
