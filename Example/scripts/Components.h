#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Простая структура для Axis-Aligned Bounding Box (AABB)
// заменяет sf::FloatRect
struct AABB
{
	float x = 0.0f, y = 0.0f;
	float width = 0.0f, height = 0.0f;

	bool intersects(const AABB& other) const
	{
		return (x < other.x + other.width && x + width > other.x && y < other.y + other.height && y + height > other.y);
	}
};

struct Position
{
	glm::vec2 pos{ 0.0f, 0.0f };
};

struct Velocity
{
	glm::vec2 vel{ 0.0f, 0.0f };
};

// Renderable больше не хранит объект SFML,
// а только данные, необходимые для рендеринга.
struct Renderable
{
	glm::vec2 size{ 50.0f, 50.0f };
	glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA
};

struct Collider
{
	AABB box;
};

// Input остается прежним, так как это просто структура с флагами.
struct Input
{
	bool moveLeft = false;
	bool moveRight = false;
	bool moveUp = false;
	bool moveDown = false;
};

// Camera больше не хранит указатель на sf::View.
// Теперь это просто компонент-маркер.
struct Camera
{
};
