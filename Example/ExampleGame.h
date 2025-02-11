#pragma once

#include "../ECS/ecs.hpp"
#include "SFML/Graphics.hpp"

//scopes
struct render {};

//components
struct Velocity { float vx, vy; };
struct Position
{
	Position(float x, float y) : x(x), y(y) {}
	Position(sf::Vector2f position) : x(position.x), y(position.y) {}

	float x, y;
};
struct Renderable
{
	Renderable(sf::Color color) : rect({ 50, 50 })
	{
		rect.setFillColor(color);
	}
	sf::RectangleShape rect;
};
struct Input
{
	bool moveLeft = false;
	bool moveRight = false;
	bool moveUp = false;
	bool moveDown = false;
};
struct Camera { sf::View& camera; sf::RenderWindow& window; };
struct Window { sf::RenderWindow& window; };

//systems
void Draw(ecs::context& ctx, Window& w)
{
	for (auto& entity : ctx.entity().at_scope<render>().all())
	{
		auto pos = entity->get<Position>();
		auto rect = entity->get<Renderable>();
		if (pos && rect)
		{
			rect->rect.setPosition(pos->x, pos->y);
			w.window.draw(rect->rect);
		}
	}
}

void Move(ecs::context& ctx, Position& p, Velocity& v)
{
	p.x += v.vx * ctx.delta_time;
	p.y += v.vy * ctx.delta_time;
}

void HandleInput(Input& input, Velocity& v)
{
	v.vx = 0.f;
	v.vy = 0.f;

	if (input.moveLeft) v.vx -= 500.f;
	if (input.moveRight) v.vx += 500.f;
	if (input.moveUp) v.vy -= 500.f;
	if (input.moveDown) v.vy += 500.f;
}

void MoveCamera(Camera& c, Position& p)
{
	auto pos = sf::Vector2f(p.x, p.y);
	auto& camera = c.camera;
	camera.setCenter(camera.getCenter() + (pos - camera.getCenter()) * 0.01f);
	c.window.setView(camera);
}

class A
{
public:
	void DoWithContext(ecs::context& ctx, Position& p)
	{
		std::cout << "Context method of A was executed. Delta Time = " << ctx.delta_time << std::endl;
	}

	void DoWithoutContext(Position& p)
	{
		std::cout << "No context method of A was executed " << m_count++ << " times" << std::endl;
	}

private:
	size_t m_count = 0;
};

//game class
class ExampleGame
{
public:
	void Start()
	{
		ecs::entity_manager em;
		ecs::system_manager sm(em);
		ecs::looper looper(sm);
		A a;

		sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "ECS Example");
		sf::View camera = window.getView();
		sf::Event event{};
		sf::Clock clock;

		auto& playerEntity = em.at_scope<render>()
			.create()
			.add<Velocity>(0.f, 0.f)
			.add<Position>(static_cast<sf::Vector2f>(window.getSize() / 2u))
			.add<Renderable>(sf::Color::Green)
			.add<Camera>(camera, window)
			.add<Input>();

		//create entity in default scope
		em.create()
			.add<Window>(window);

		em.at_scope<render>()
			.create()
			.add<Position>(100.f, 100.f)
			.add<Renderable>(sf::Color::Red);

		sm.system<Window>("Draw")
			.each(Draw, true);

		sm.system<Position>("DoWithContext")
			.each(&A::DoWithContext, a, true);

		sm.system<Position>("DoWithoutContext")
			.each(&A::DoWithoutContext, a);

		sm.system<Position, Velocity>("Move")
			.each(Move, true);

		sm.system<Input, Velocity>("HandleInput")
			.each(HandleInput);

		sm.system<Camera, Position>("MoveCamera")
			.each(MoveCamera);

		while (window.isOpen())
		{
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) window.close();
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::A) playerEntity.get<Input>()->moveLeft = true;
					if (event.key.code == sf::Keyboard::D) playerEntity.get<Input>()->moveRight = true;
					if (event.key.code == sf::Keyboard::W) playerEntity.get<Input>()->moveUp = true;
					if (event.key.code == sf::Keyboard::S) playerEntity.get<Input>()->moveDown = true;
				}
				if (event.type == sf::Event::KeyReleased)
				{
					if (event.key.code == sf::Keyboard::A) playerEntity.get<Input>()->moveLeft = false;
					if (event.key.code == sf::Keyboard::D) playerEntity.get<Input>()->moveRight = false;
					if (event.key.code == sf::Keyboard::W) playerEntity.get<Input>()->moveUp = false;
					if (event.key.code == sf::Keyboard::S) playerEntity.get<Input>()->moveDown = false;
				}
			}

			float dt = clock.restart().asSeconds();
			window.clear();
			looper.frame(dt);
			window.display();
		}
	}
};