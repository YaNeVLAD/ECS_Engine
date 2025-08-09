#pragma once

#define ENGINE_FOLD_NAMESPACES

#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include <GLFW/glfw3.h>

#include <Application.hpp>
#include <ecs.hpp>
#include <physics.hpp>
#include <scripts.hpp>

#include "PlayerController.h"

using namespace Engine;
using namespace Engine::physics;
using namespace Engine::physics::components;
using namespace Engine::scripts;
using namespace Engine::render;

struct Renderable
{
	glm::vec2 size{ 50.0f, 50.0f };
	glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

class GameApplication : public Engine::Application
{
public:
	~GameApplication() = default;

	void OnCreate() override
	{
		auto& scene = Scene();

		// --- 1. Register all components and systems with the ECS ---
		scene.RegisterComponents<Transform, Input, RigidBody, Renderable, AABBCollider, ScriptComponent>();

		scene.RegisterSystem<PhysicsSystem>()
			.WithRead<Transform>()
			.WithRead<RigidBody>()
			.WithRead<AABBCollider>();

		scene.RegisterSystem<ScriptingSystem>()
			.WithRead<ScriptComponent>();

		scene.BuildSystemGraph();

		// --- 2. Create game entities ---
		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> pos_dist(100.f, 1820.f);
		std::uniform_real_distribution<float> vel_dist(-100.f, 100.f);
		std::uniform_real_distribution<float> mass_dist(100.f, 300.f);
		const int ENTITY_COUNT = 10;

		for (int i = 0; i < ENTITY_COUNT; ++i)
		{
			glm::vec2 size = { 50.f, 50.f };
			math::Vector2 size2 = { size.x, size.y };
			ecs::Entity entity = scene.CreateEntity();
			scene.AddComponent<Transform>(entity, { pos_dist(rng), pos_dist(rng) });
			scene.AddComponent<RigidBody>(entity,
				RigidBody{
					.Velocity = { vel_dist(rng), vel_dist(rng) },
					.Mass = mass_dist(rng) });
			scene.AddComponent<AABBCollider>(entity, size2, size2);
			scene.AddComponent<Renderable>(entity, Renderable{ size });
		}

		m_player = scene.CreateEntity();
		scene.AddComponent<Transform>(m_player, Transform{ .Position = { 1920.f / 2, 1080.f / 2 } });
		scene.AddComponent<RigidBody>(m_player, RigidBody{ .Mass = 1000.f });
		scene.AddComponent<AABBCollider>(m_player);
		scene.AddComponent<Renderable>(m_player, Renderable{ .color = { 1.f, 0.f, 0.f, 1.f } });
		scene.AddComponent<Input>(m_player);
		scene.AddComponent<ScriptComponent>(m_player);

		scripts::Bind<PlayerController>(scene, m_player);
	}

	void OnUpdate(float dt) override
	{
		auto& scene = Scene();
		auto* window = (GLFWwindow*)Window().GetNativeWindow();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			Close();
		}

		auto& playerInput = scene.GetComponent<Input>(m_player);
		playerInput.moveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
		playerInput.moveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
		playerInput.moveUp = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		playerInput.moveDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;

		// --- 2. Update game state (ECS systems) ---
		scene.Frame(dt);
		scene.ConfirmChanges();

		// --- 3. Render the scene ---
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 projection = glm::ortho(0.0f, (float)Window().Width(), 0.0f, (float)Window().Height(), -1.0f, 1.0f);
		glm::mat4 view = glm::mat4(1.0f);

		Renderer2D::BeginScene(projection, view);

		auto renderView = scene.CreateView<Transform, Renderable>();

		for (const auto& [entity, transform, renderable] : *renderView)
		{
			glm::vec2 pos = { transform.Position.X, transform.Position.Y };
			Renderer2D::DrawQuad(pos, renderable.size, renderable.color);
		}

		Renderer2D::EndScene();

		// --- 4. Update window title with FPS ---
		std::string title = "ECS Demo | FPS: " + std::to_string(static_cast<int>(1.f / dt));
		Window().SetTitle(title);
	}

	void OnDestroy() override
	{
	}

private:
	ecs::Entity m_player = InvalidEntity;
};

Engine::Application* Engine::CreateApplication()
{
	return new GameApplication();
}
