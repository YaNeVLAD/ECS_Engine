#include <cmath>
#include <numeric>
#include <random>

#include <SFML/Graphics.hpp>

#define ENTT 0

#if ENTT
#include "Example/entt/Scene.h"
#else
#include <ecs.hpp>
#endif

#include "Example/physics/Game.h"
#include "Timer.h"

namespace ecs
{
}

using namespace ecs;

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

struct Health
{
	int value = 100;
};
struct Damage
{
	int amount = 0;
};

std::unordered_map<const char*, long long> g_system_timings;

class PhysicsSystem : public System
{
public:
	void Update(Scene& scene, float dt) override
	{
#if ENTT
		auto Entities = scene.CreateView<Transform, Velocity, RigidBody>();
#else
		auto& Entities = *scene.CreateView<Transform, Velocity, RigidBody>();
#endif

		auto start = std::chrono::high_resolution_clock::now();

#if ENTT
		for (auto [entity, transform, velocity, body] : Entities.each())
#else
		for (auto&& [entity, transform, velocity, body] : Entities)
#endif
		{
			transform.x += velocity.dx * dt;
			transform.y += velocity.dy * dt;
			transform.z += velocity.dz * dt * std::sin(body.mass);
		}

		auto end = std::chrono::high_resolution_clock::now();
		g_system_timings[typeid(*this).name()] += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}
};

class DamageSystem : public System
{
public:
	void Update(Scene& world, float dt) override
	{
#if ENTT
		auto Entities = world.CreateView<Health, Damage>();
#else
		auto& Entities = *world.CreateView<Health, Damage>();
#endif

		auto type = typeid(DamageSystem).name();

		auto start = std::chrono::high_resolution_clock::now();

#if ENTT
		for (auto [entity, health, damage] : Entities.each())
#else
		for (auto&& [entity, health, damage] : Entities)
#endif
		{
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

class RenderDataSystem : public System
{
public:
	void Update(Scene& world, float dt) override
	{
#if ENTT
		auto Entities = world.CreateView<Transform>();
#else
		auto& Entities = *world.CreateView<Transform>();
#endif

		m_drawData.clear();
#if ENTT
		m_drawData.reserve(Entities.size());
#endif

#if ENTT
		for (const auto& [entity, transform] : Entities.each())
#else
		for (auto&& [entity, transform] : Entities)
#endif
		{
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
			// window.draw(rect);
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

	Scene world;

	{
		Timer timer("Setup: Registering components and systems");

		world.RegisterComponent<Transform>();
		world.RegisterComponent<Velocity>();
		world.RegisterComponent<RigidBody>();
		world.RegisterComponent<Renderable>();
		world.RegisterComponent<Health>();
		world.RegisterComponent<Damage>();

		world.RegisterSystem<PhysicsSystem>();
		//.WithRead<Velocity>()
		//.WithRead<RigidBody>()
		//.WithWrite<Transform>();

		world.RegisterSystem<DamageSystem>();
		//.WithRead<Damage>()
		//.WithWrite<Health>();

		world.RegisterSystem<RenderDataSystem>();
		//.WithRead<Transform>()
		//.WithRead<Renderable>();

		world.BuildSystemGraph();
	}

	{
		Timer timer("Setup: Creating entities");

		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

		for (int i = 0; i < ENTITY_COUNT; ++i)
		{
			Entity entity = world.CreateEntity();
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

	RunGame();

	return 0;
}
