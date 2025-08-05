#pragma once

#include <random>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define PROFILE 0
#if PROFILE
#include <tracy/Tracy.hpp>
#else
#define FrameMark
#define ZoneScoped
#endif

#include "../../Engine/src/ECS/Scene/Scene.h"
#include "../../Engine/src/Script/native/ScriptComponent.h"
#include "../../Engine/src/Script/native/ScriptingSystem.h"

#include "Components.h"
#include "PlayerController.h"

class MovementSystem : public ecs::System
{
public:
	void Update(ecs::Scene& world, float dt) override
	{
		ZoneScoped;
		for (auto& entity : Entities)
		{
			auto& position = entity.GetComponent<Position>();
			const auto& velocity = entity.GetComponent<Velocity>();

			position.pos.x += velocity.vel.x * dt;
			position.pos.y += velocity.vel.y * dt;
		}
	}
};

class ColliderUpdateSystem : public ecs::System
{
public:
	void Update(ecs::Scene& world, float dt) override
	{
		ZoneScoped;
		for (auto& entity : Entities)
		{
			const auto& position = entity.GetComponent<Position>();
			auto& collider = entity.GetComponent<Collider>();

			collider.box.x = position.pos.x;
			collider.box.y = position.pos.y;
		}
	}
};

class CollisionSystem : public ecs::System
{
	using RenderView = ecs::View<Collider, Renderable>;

public:
	CollisionSystem(ecs::Scene& world)
	{
		m_allCollidables = world.CreateView<Collider, Renderable>();
	}

	void Update(ecs::Scene& world, float dt) override
	{
		ZoneScoped;
		for (auto&& [entity, collider, renderable] : *m_allCollidables)
		{
			renderable.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

		for (auto& entity : Entities)
		{
			auto& renderable = entity.GetComponent<Renderable>();
			const auto& collider = entity.GetComponent<Collider>();

			for (auto [otherEntity, otherCollider, otherRenderable] : *m_allCollidables)
			{
				if (entity == otherEntity)
					continue;

				if (collider.box.intersects(otherCollider.box))
				{
					renderable.color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
				}
			}
		}
	}

private:
	std::shared_ptr<RenderView> m_allCollidables;
};

class CameraSystem : public ecs::System
{
public:
	CameraSystem(glm::vec2 startPos, glm::mat4& viewMatrix, int screenWidth, int screenHeight)
		: m_viewMatrix(viewMatrix)
		, m_cameraCenter(startPos)
		, m_screenWidth(screenWidth)
		, m_screenHeight(screenHeight)
	{
	}

	void Update(ecs::Scene& world, float dt) override
	{
		for (auto& entity : Entities)
		{
			const auto& position = entity.GetComponent<Position>();

			m_cameraCenter += (position.pos - m_cameraCenter) * 0.05f;

			glm::vec3 eye = { m_cameraCenter.x, m_cameraCenter.y, 1.0f };
			glm::vec3 center = { m_cameraCenter.x, m_cameraCenter.y, 0.0f };
			glm::vec3 up = { 0.0f, 1.0f, 0.0f };

			glm::mat4 lookAtMatrix = glm::lookAt(eye, center, up);

			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_screenWidth / 2, m_screenHeight / 2, 0.0f));

			m_viewMatrix = translateMatrix * lookAtMatrix;
		}
	}

private:
	glm::mat4& m_viewMatrix;
	glm::vec2 m_cameraCenter;
	int m_screenWidth;
	int m_screenHeight;
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
			-0.5f, 0.5f, // Верхний левый
			0.5f, -0.5f, // Нижний правый
			-0.5f, -0.5f, // Нижний левый
			0.5f, 0.5f // Верхний правый
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

	void Draw(ecs::View<Position, Renderable> const& view, glm::mat4 const& projection, glm::mat4 const& viewMatrix) const
	{
		ZoneScoped;
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(m_shaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, &viewMatrix[0][0]);

		glBindVertexArray(m_quadVAO);

		for (const auto& [entity, position, renderable] : view)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position.pos, 0.0f));
			model = glm::scale(model, glm::vec3(renderable.size, 1.0f));

			glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
			glUniform4fv(glGetUniformLocation(m_shaderProgram, "objectColor"), 1, &renderable.color[0]);

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
};

class Game
{
public:
	void Run()
	{
		const int SCREEN_WIDTH = 1920;
		const int SCREEN_HEIGHT = 1080;
		const int ENTITY_COUNT = 10;

		// --- Инициализация GLFW и GLAD ---
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
		glfwSwapInterval(1); // v-sync on

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		// --- Шейдеры ---
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

		// --- ECS и Рендерер ---
		ecs::Scene world;
		Renderer renderer;
		renderer.Init(vsSource, fsSource);

		world.RegisterComponents<Position, Velocity, Renderable, Input, Camera, Collider, ecs::ScriptComponent>();

		// Матрица вида, которая будет обновляться CameraSystem
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		glm::vec2 playerStartPos = { 960.f, 540.f };

		world.RegisterSystem<MovementSystem>()
			.WithRead<Velocity>()
			.WithWrite<Position>();
		world.RegisterSystem<ColliderUpdateSystem>()
			.WithRead<Position>()
			.WithWrite<Collider>();
		world.RegisterSystem<CollisionSystem>(world)
			.WithRead<Collider>()
			.WithWrite<Renderable>();
		world.RegisterSystem<CameraSystem>(playerStartPos, viewMatrix, SCREEN_WIDTH, SCREEN_HEIGHT)
			.WithRead<Position>()
			.WithWrite<Camera>();
		world.RegisterSystem<ecs::ScriptingSystem>()
			.WithRead<ecs::ScriptComponent>();

		world.BuildSystemGraph();

		// --- Создание сущностей ---
		ecs::Entity playerEntity = world.CreateEntity();
		world.AddComponent<Position>(playerEntity, { playerStartPos });
		world.AddComponent<Velocity>(playerEntity, {});
		world.AddComponent<Renderable>(playerEntity, { { 50.f, 50.f }, { 0.f, 1.f, 0.f, 1.f } }); // Green
		world.AddComponent<Input>(playerEntity, {});
		world.AddComponent<Camera>(playerEntity, {});
		world.AddComponent<Collider>(playerEntity, { { playerStartPos.x, playerStartPos.y, 50.f, 50.f } });
		world.AddComponent<ecs::ScriptComponent>(playerEntity, {});
		world.GetComponent<ecs::ScriptComponent>(playerEntity).Bind<PlayerController>(world, playerEntity);

		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> pos_dist(300.f, 1000.f);
		for (int i = 0; i < ENTITY_COUNT; ++i)
		{
			float x = pos_dist(rng);
			float y = pos_dist(rng);
			auto entity = world.CreateEntity();
			world.AddComponent<Position>(entity, { { x, y } });
			world.AddComponent<Renderable>(entity, { { 50.f, 50.f }, { 1.f, 0.f, 0.f, 1.f } }); // Red
			world.AddComponent<Collider>(entity, { { x, y, 50.f, 50.f } });
		}

		auto renderView = world.CreateView<Position, Renderable>();

		// --- Главный цикл ---
		auto lastTime = std::chrono::high_resolution_clock::now();

		while (!glfwWindowShouldClose(window))
		{
			auto currentTime = std::chrono::high_resolution_clock::now();
			float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
			lastTime = currentTime;

			// --- Обработка ввода ---
			glfwPollEvents();
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);

			auto& playerInput = world.GetComponent<Input>(playerEntity);
			playerInput.moveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
			playerInput.moveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
			playerInput.moveUp = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
			playerInput.moveDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;

			// --- Обновление мира ---
			world.TakeStep(dt);
			world.ConfirmChanges();

			// --- Рендеринг ---
			// Ортографическая проекция, совпадающая с размерами окна
			glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

			renderer.Draw(*renderView, projection, viewMatrix);

			glfwSwapBuffers(window);

			std::string title = "FPS: " + std::to_string(static_cast<int>(1.f / dt));
			glfwSetWindowTitle(window, title.c_str());

			FrameMark;
		}

		// --- Очистка ---
		renderer.Shutdown();
		glfwTerminate();
	}
};
