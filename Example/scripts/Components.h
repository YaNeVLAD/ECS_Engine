#pragma once

#include <SFML/Graphics.hpp>

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
