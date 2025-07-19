#include <cmath>
#include <numeric>
#include <random>

#include "Timer.h"

#include "Engine/src/ECS/World/World.h"

#include <SFML/Graphics.hpp>

#include "Example/NewExample.h"

#define BENCHMARK_ON 0

#if BENCHMARK_ON
struct Transform
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
};
struct Velocity
{
	float dx = 0.0f, dy = 0.0f, dz = 0.0f;
};

struct RigidBody
{
	double mass = 1.0;
};

struct Renderable
{
	Renderable()
		: rect()
	{
		rect.setFillColor(sf::Color::Red);
		rect.setSize({ 50.f, 50.f });
	}

	sf::RectangleShape rect;
};

struct Window
{
	sf::RenderWindow* window;
};

struct Health
{
	int value = 100;
};
struct Damage
{
	int amount = 0;
};

std::unordered_map<const char*, long long> g_system_timings;

class PhysicsSystem : public ecs::System
{
public:
	void Update(ecs::World& world, float dt) override
	{
		auto start = std::chrono::high_resolution_clock::now();

		for (const auto& entity : Entities)
		{
			auto& transform = world.GetComponent<Transform>(entity);
			const auto& velocity = world.GetComponent<Velocity>(entity);
			const auto& body = world.GetComponent<RigidBody>(entity);

			transform.x += velocity.dx * dt;
			transform.y += velocity.dy * dt;
			transform.z += velocity.dz * dt * std::sin(body.mass);
		}

		auto end = std::chrono::high_resolution_clock::now();
		g_system_timings[typeid(*this).name()] += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}
};

class DamageSystem : public ecs::System
{
public:
	void Update(ecs::World& world, float dt) override
	{
		auto type = typeid(DamageSystem).name();

		auto start = std::chrono::high_resolution_clock::now();

		for (const auto& entity : Entities)
		{
			auto& health = world.GetComponent<Health>(entity);
			const auto& damage = world.GetComponent<Damage>(entity);
			if (damage.amount > 0)
			{
				health.value -= damage.amount;
			}
			if (health.value < 0)
			{
				world.DestoryEntity(entity);
			}
		}

		auto end = std::chrono::high_resolution_clock::now();
		g_system_timings[typeid(*this).name()] += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}
};

struct DrawData
{
	float x, y;
};

class RenderDataSystem : public ecs::System
{
public:
	void Update(ecs::World& world, float dt) override
	{
		m_drawData.clear();
		m_drawData.reserve(Entities.size());

		for (const auto& entity : Entities)
		{
			auto const& transform = world.GetComponent<Transform>(entity);
			m_drawData.emplace_back(transform.x, transform.y);
		}
	}

	const std::vector<DrawData>& GetDrawData() const
	{
		return m_drawData;
	}

private:
	std::vector<DrawData> m_drawData;
};

class Renderer
{
public:
	void Draw(sf::RenderWindow& window, RenderDataSystem& renderDataSystem)
	{
		window.clear(sf::Color::Black);

		sf::RectangleShape rect({ 50.f, 50.f });
		rect.setFillColor(sf::Color::Red);

		for (const auto& data : renderDataSystem.GetDrawData())
		{
			rect.setPosition(data.x, data.y);
			window.draw(rect);
		}

		window.display();
	}
};
#endif

int main()
{
#if BENCHMARK_ON
	const int ENTITY_COUNT = 100'00;
	const int BENCHMARK_SECONDS = 10;
	const float FIXED_DT = 1.0f / 60.0f;

	std::cout << "--- ECS Benchmark ---" << std::endl;
	std::cout << "Entities: " << ENTITY_COUNT << std::endl;
	std::cout << "Duration: " << BENCHMARK_SECONDS << " seconds" << std::endl;
	std::cout << "_____________________" << std::endl;

	ecs::World world;

	{
		Timer timer("Setup: Registering components and systems");

		world.RegisterComponent<Transform>();
		world.RegisterComponent<Velocity>();
		world.RegisterComponent<RigidBody>();
		world.RegisterComponent<Renderable>();
		world.RegisterComponent<Health>();
		world.RegisterComponent<Damage>();

		world.RegisterSystem<PhysicsSystem>()
			.WithRead<Velocity>()
			.WithRead<RigidBody>()
			.WithWrite<Transform>();

		world.RegisterSystem<DamageSystem>()
			.WithRead<Damage>()
			.WithWrite<Health>();

		world.RegisterSystem<RenderDataSystem>()
			.WithRead<Transform>()
			.WithRead<Renderable>();

		world.BuildSystemGraph();
	}

	{
		Timer timer("Setup: Creating entities");

		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

		for (int i = 0; i < ENTITY_COUNT; ++i)
		{
			ecs::Entity entity = world.CreateEntity();
			world.AddComponent<Transform>(entity, { dist(rng), dist(rng), dist(rng) });
			world.AddComponent<Velocity>(entity, { dist(rng), dist(rng), dist(rng) });
			world.AddComponent<RigidBody>(entity, { dist(rng) / 100.0 });
			world.AddComponent<Renderable>(entity, {});
			world.AddComponent<Health>(entity, { 100 });
			world.AddComponent<Damage>(entity, { i % 10 == 0 ? 1 : 0 });
		}
	}

	std::cout << "\n--- Running Benchmark for " << BENCHMARK_SECONDS << " seconds... ---" << std::endl;

	size_t benchmarkFrames = 0;
	size_t frameCount = 0;
	auto benchmarkStart = std::chrono::high_resolution_clock::now();
	auto loopEnd = benchmarkStart + std::chrono::seconds(BENCHMARK_SECONDS);

	sf::Event event;
	sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "ECS Benchmark");
	Renderer renderer;
	sf::Clock clock;
	auto lastFPSTime = std::chrono::high_resolution_clock::now();

	auto renderView = world.CreateView<Transform, Renderable>();

	while (window.isOpen() && std::chrono::high_resolution_clock::now() < loopEnd)
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		auto dt = clock.restart().asSeconds();

		world.TakeStep(dt);

		world.ConfirmChanges();

		renderer.Draw(window, world.GetSystem<RenderDataSystem>());

		frameCount++;
		benchmarkFrames++;

		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = currentTime - lastFPSTime;

		if (elapsed.count() >= 1.0f)
		{
			float fps = static_cast<float>(frameCount) / elapsed.count();
			std::cout << "FPS: " << fps << std::endl;
			frameCount = 0;
			lastFPSTime = currentTime;
		}
	}
	auto benchmarkEnd = std::chrono::high_resolution_clock::now();
	auto totalDuration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(benchmarkEnd - benchmarkStart).count();

	std::cout << "\n--- Benchmark Results ---" << std::endl;
	std::cout << "Total time: " << totalDuration_ms / 1000.0 << "s" << std::endl;
	std::cout << "Total frames: " << benchmarkFrames << std::endl;
	std::cout << "Average FPS: " << static_cast<double>(benchmarkFrames) / (totalDuration_ms / 1000.0) << std::endl;
	std::cout << "Average frame time: " << static_cast<double>(totalDuration_ms) / benchmarkFrames << " ms" << std::endl;
	std::cout << "_____________________" << std::endl;
	std::cout << "System Performance:" << std::endl;

	long long totalSystemTime_us = 0;
	for (const auto& [name, timing] : g_system_timings)
	{
		totalSystemTime_us += timing;
	}

	for (const auto& [name, timing_us] : g_system_timings)
	{
		double percentage = (static_cast<double>(timing_us) / totalSystemTime_us) * 100.0;
		std::cout << " - " << name << ":\n";
		std::cout << "   Total time: " << timing_us / 1000.0 << " ms\n";
		std::cout << "   Avg time/frame: " << static_cast<double>(timing_us) / frameCount << " us\n";
		std::cout << "   Contribution: " << percentage << "%\n";
	}
#endif

	Game game;
	game.Run();

	return 0;
}
