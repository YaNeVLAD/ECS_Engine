#include "Color.h"

namespace Engine::render
{

const Color Color::Red(255, 0, 0);
const Color Color::Green(0, 255, 0);
const Color Color::Blue(0, 0, 255);
const Color Color::Magenta(255, 0, 255);
const Color Color::Cyan(0, 255, 255);
const Color Color::White(255, 255, 255);
const Color Color::Black(0, 0, 0);
const Color Color::Yellow(255, 255, 0);
const Color Color::Transparent(0, 0, 0, 0);

Color::Color()
	: r(0)
	, g(0)
	, b(0)
	, a(255)
{
}

Color::Color(uint32_t hex)
	: r((hex & 0xFF000000) >> 24)
	, g((hex & 0x00FF0000) >> 16)
	, b((hex & 0x0000FF00) >> 8)
	, a((hex & 0x000000FF) >> 0)
{
}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
{
}

uint32_t Color::ToInt() const
{
	return (r << 24) | (g << 16) | (b << 8) | a;
}

} // namespace Engine::render
