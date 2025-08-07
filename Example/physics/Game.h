#pragma once

#define ENGINE_FOLD_NAMESPACES

#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ecs.hpp>
#include <physics.hpp>
#include <scripts.hpp>

#include "PlayerController.h"

struct Renderable
{
	glm::vec2 size{ 50.0f, 50.0f };
	glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

class Renderer
{
public:
	void Init(const char* vsSource, const char* fsSource)
	{
		// Компиляция шейдеров
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vsSource, NULL);
		glCompileShader(vertexShader);
		// (Проверка ошибок компиляции)

		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fsSource, NULL);
		glCompileShader(fragmentShader);
		// (Проверка ошибок компиляции)

		m_shaderProgram = glCreateProgram();
		glAttachShader(m_shaderProgram, vertexShader);
		glAttachShader(m_shaderProgram, fragmentShader);
		glLinkProgram(m_shaderProgram);
		// (Проверка ошибок линковки)
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		float vertices[] = {
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 1.0f
		};

		unsigned int indices[] = {
			0, 2, 1,
			0, 1, 3
		};

		glGenVertexArrays(1, &m_quadVAO);
		glGenBuffers(1, &m_quadVBO);
		glGenBuffers(1, &m_quadEBO);

		glBindVertexArray(m_quadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Draw(ecs::View<ecs::physics::components::Transform, Renderable, ecs::physics::components::AABBCollider> const& view, glm::mat4 const& projection, glm::mat4 const& viewMatrix) const
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(m_shaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, &viewMatrix[0][0]);

		glBindVertexArray(m_quadVAO);

		for (const auto& [entity, transform, renderable, collider] : view)
		{
			ecs::math::Vector2 size = { renderable.size.x, renderable.size.y };
			DrawRect(transform.Position, size, { 1.f, 0.f, 0.f, 1.f });

			// DrawRect(collider.Position, collider.Size, { 0.f, 1.f, 0.f, 1.f });

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);
	}

	void Shutdown() const
	{
		glDeleteVertexArrays(1, &m_quadVAO);
		glDeleteBuffers(1, &m_quadVBO);
		glDeleteBuffers(1, &m_quadEBO);
		glDeleteProgram(m_shaderProgram);
	}

private:
	unsigned int m_shaderProgram;
	unsigned int m_quadVAO, m_quadVBO, m_quadEBO;

	void DrawRect(
		ecs::math::Vector2 const& position,
		ecs::math::Vector2 const& size,
		glm::vec4 const& color) const
	{
		glm::vec2 vec = { position.X, position.Y };
		glm::vec2 sz = { size.X, size.Y };
		glm::vec3 pos = { vec, 0.f };
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		model = glm::scale(model, glm::vec3(sz, 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
		glUniform4fv(glGetUniformLocation(m_shaderProgram, "objectColor"), 1, &color[0]);
	}
};

void RunGame()
{
	using namespace ecs::physics;
	using namespace ecs::physics::components;

	const int SCREEN_WIDTH = 1920;
	const int SCREEN_HEIGHT = 1080;
	const int ENTITY_COUNT = 10;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "New ECS Demo (GLFW)", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	const char* vsSource = R"glsl(
			#version 330 core
			layout (location = 0) in vec2 aPos;

			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;

			void main()
			{
				gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
			}
		)glsl";
	const char* fsSource = R"glsl(
			#version 330 core
			out vec4 FragColor;

			uniform vec4 objectColor;

			void main()
			{
				FragColor = objectColor;
			}
		)glsl";

	ecs::Scene world;
	Renderer renderer;
	renderer.Init(vsSource, fsSource);

	world.RegisterComponents<
		Transform,
		Input,
		RigidBody,
		Renderable,
		AABBCollider,
		ecs::ScriptComponent>();

	world.RegisterSystem<PhysicsSystem>()
		.WithRead<Transform>()
		.WithRead<RigidBody>()
		.WithRead<AABBCollider>();

	world.RegisterSystem<ecs::ScriptingSystem>()
		.WithRead<ecs::ScriptComponent>();

	world.BuildSystemGraph();

	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> pos_dist(100.f, 1820.f);
	std::uniform_real_distribution<float> vel_dist(-100.f, 100.f);
	std::uniform_real_distribution<float> mass_dist(100.f, 300.f);

	for (int i = 0; i < ENTITY_COUNT; ++i)
	{
		glm::vec2 size = { 50.f, 50.f };
		ecs::math::Vector2 size2 = { 50.f, 50.f };
		ecs::Entity entity = world.CreateEntity();
		world.AddComponent<Transform>(entity, { pos_dist(rng), pos_dist(rng) });
		world.AddComponent<RigidBody>(entity,
			RigidBody{
				.Velocity = { vel_dist(rng), vel_dist(rng) },
				.Mass = mass_dist(rng) });
		world.AddComponent<AABBCollider>(entity, size2, size2);
		world.AddComponent<Renderable>(entity, size);
	}

	ecs::Entity player = world.CreateEntity();
	world.AddComponent<Transform>(player, Transform{ .Position = { 1920.f / 2, 1080.f / 2 } });
	world.AddComponent<RigidBody>(player, RigidBody{ .Mass = 1000.f });
	world.AddComponent<AABBCollider>(player);
	world.AddComponent<Renderable>(player, Renderable{ .color = { 1.f, 0.f, 0.f, 1.f } });
	world.AddComponent<Input>(player);
	world.AddComponent<ecs::ScriptComponent>(player);
	ecs::Bind<PlayerController>(world, player);

	auto renderView = world.CreateView<Transform, Renderable, AABBCollider>();

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	auto lastTime = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(window))
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;

		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		auto& playerInput = world.GetComponent<Input>(player);
		playerInput.moveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
		playerInput.moveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
		playerInput.moveUp = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		playerInput.moveDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;

		world.TakeStep(dt);
		world.ConfirmChanges();

		glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

		renderer.Draw(*renderView, projection, viewMatrix);

		glfwSwapBuffers(window);

		std::string title = "FPS: " + std::to_string(static_cast<int>(1.f / dt));
		glfwSetWindowTitle(window, title.c_str());
	}

	renderer.Shutdown();
	glfwTerminate();
}
