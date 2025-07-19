#pragma once

#include "../Engine/src/ECS/World/World.h"
#include "SFML/Graphics.hpp"
#include <random>

struct Position
{
	float x = 0.0f, y = 0.0f;
};

struct Velocity
{
	float vx = 0.0f, vy = 0.0f;
};

struct Renderable
{
	Renderable(sf::Color color)
		: rect({ 50.f, 50.f })
	{
		rect.setFillColor(color);
	}
	sf::RectangleShape rect;
};

struct Input
{
	bool moveLeft = false, moveRight = false, moveUp = false, moveDown = false;
};

struct Camera
{
	sf::View* view;
};

struct Collider
{
	sf::FloatRect rect;
};

class HandleInputSystem : public ecs::System
{
public:
	void Update(ecs::World& world, float dt) override
	{
		for (const auto& entity : Entities)
		{
			const auto& input = world.GetComponent<Input>(entity);
			auto& velocity = world.GetComponent<Velocity>(entity);

			velocity.vx = 0.f;
			velocity.vy = 0.f;

			if (input.moveLeft)
				velocity.vx -= 500.f;
			if (input.moveRight)
				velocity.vx += 500.f;
			if (input.moveUp)
				velocity.vy -= 500.f;
			if (input.moveDown)
				velocity.vy += 500.f;
		}
	}
};

class MovementSystem : public ecs::System
{
public:
	void Update(ecs::World& world, float dt) override
	{
		for (const auto& entity : Entities)
		{
			auto& position = world.GetComponent<Position>(entity);
			const auto& velocity = world.GetComponent<Velocity>(entity);

			position.x += velocity.vx * dt;
			position.y += velocity.vy * dt;
		}
	}
};

class ColliderUpdateSystem : public ecs::System
{
public:
	void Update(ecs::World& world, float dt) override
	{
		for (const auto& entity : Entities)
		{
			const auto& position = world.GetComponent<Position>(entity);
			auto& collider = world.GetComponent<Collider>(entity);

			collider.rect.left = position.x;
			collider.rect.top = position.y;
		}
	}
};

class CollisionSystem : public ecs::System
{
public:
	CollisionSystem(ecs::World& world)
	{
		m_allCollidables = world.CreateView<Collider, Renderable>();
	}

	void Update(ecs::World& world, float dt) override
	{
		for (auto [entity, collider, renderable] : m_allCollidables->Each())
		{
			renderable.rect.setFillColor(sf::Color::Red);
		}

		for (const auto& entity : Entities)
		{
			auto& renderable = world.GetComponent<Renderable>(entity);
			auto& collider = world.GetComponent<Collider>(entity);

			for (auto [otherEntity, otherCollider, otherRenderable] : m_allCollidables->Each())
			{
				if (entity == otherEntity)
					continue;

				if (collider.rect.intersects(otherCollider.rect))
				{
					renderable.rect.setFillColor(sf::Color::Cyan);
				}
			}
		}
	}

private:
	std::shared_ptr<ecs::View<Collider, Renderable>> m_allCollidables;
};

class CameraSystem : public ecs::System
{
public:
	CameraSystem(sf::RenderWindow& window)
		: m_window(window)
	{
	}

	void Update(ecs::World& world, float dt) override
	{
		for (const auto& entity : Entities)
		{
			const auto& position = world.GetComponent<Position>(entity);
			auto& cameraComponent = world.GetComponent<Camera>(entity);

			sf::View* cameraView = cameraComponent.view;
			if (cameraView)
			{
				auto targetPos = sf::Vector2f(position.x, position.y);
				cameraView->setCenter(cameraView->getCenter() + (targetPos - cameraView->getCenter()) * 0.05f);
			}
		}
	}

private:
	sf::RenderWindow& m_window;
};

class Renderer
{
public:
	void Draw(sf::RenderWindow& window, ecs::View<Position, Renderable>& view)
	{
		window.clear(sf::Color::Black);

		for (auto [entity, position, renderable] : view.Each())
		{
			renderable.rect.setPosition(position.x, position.y);
			window.draw(renderable.rect);
		}

		window.display();
	}
};

class Game
{
public:
	void Run()
	{
		const int ENTITY_COUNT = 10;

		ecs::World world;
		Renderer renderer;
		sf::RenderWindow window(sf::VideoMode(1920, 1080), "New ECS Demo");
		window.setFramerateLimit(144);

		world.RegisterComponent<Position>();
		world.RegisterComponent<Velocity>();
		world.RegisterComponent<Renderable>();
		world.RegisterComponent<Input>();
		world.RegisterComponent<Camera>();
		world.RegisterComponent<Collider>();

		world.RegisterSystem<HandleInputSystem>()
			.WithRead<Input>()
			.WithWrite<Velocity>();

		world.RegisterSystem<MovementSystem>()
			.WithRead<Velocity>()
			.WithWrite<Position>();

		world.RegisterSystem<ColliderUpdateSystem>()
			.WithRead<Position>()
			.WithWrite<Collider>();

		world.RegisterSystem<CollisionSystem>(world)
			.WithRead<Collider>()
			.WithWrite<Renderable>();

		world.RegisterSystem<CameraSystem>(window)
			.WithRead<Position>()
			.WithWrite<Camera>();

		world.BuildSystemGraph();

		sf::View cameraView = window.getDefaultView();
		ecs::Entity playerEntity = world.CreateEntity();
		world.AddComponent<Position>(playerEntity, { 960.f, 540.f });
		world.AddComponent<Velocity>(playerEntity, {});
		world.AddComponent<Renderable>(playerEntity, { sf::Color::Green });
		world.AddComponent<Input>(playerEntity, {});
		world.AddComponent<Camera>(playerEntity, { &cameraView });
		world.AddComponent<Collider>(playerEntity, { { 960.f, 540.f, 50.f, 50.f } });

		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> pos_dist(300.f, 1000.f);
		for (int i = 0; i < ENTITY_COUNT; ++i)
		{
			float x = pos_dist(rng);
			float y = pos_dist(rng);
			auto entity = world.CreateEntity();
			world.AddComponent<Position>(entity, { x, y });
			world.AddComponent<Renderable>(entity, { sf::Color::Red });
			world.AddComponent<Collider>(entity, { { x, y, 50.f, 50.f } });
		}

		auto renderView = world.CreateView<Position, Renderable>();

		sf::Clock clock;
		sf::Event event;

		while (window.isOpen())
		{
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();

				auto& playerInput = world.GetComponent<Input>(playerEntity);
				playerInput.moveLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
				playerInput.moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
				playerInput.moveUp = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
				playerInput.moveDown = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
			}

			float dt = clock.restart().asSeconds();

			world.TakeStep(dt);

			world.ConfirmChanges();

			window.setView(cameraView);

			renderer.Draw(window, *renderView);

			window.setTitle("FPS: " + std::to_string(static_cast<int>(1.f / dt)));
		}
	}
};
