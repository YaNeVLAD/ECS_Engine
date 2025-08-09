#pragma once

#include <compare>
#include <cstdint>

namespace Engine::render
{

struct Color
{
	Color();
	Color(Color&&) = default;
	Color(Color const&) noexcept = default;

	Color& operator=(Color const&) = default;
	Color& operator=(Color&&) = default;

	explicit Color(uint32_t hex);

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

	auto operator<=>(Color const&) const = default;

	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Cyan;
	static const Color Magenta;
	static const Color White;
	static const Color Black;
	static const Color Yellow;
	static const Color Transparent;

	uint32_t ToInt() const;

	explicit operator uint32_t() const
	{
		return ToInt();
	}

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

} // namespace Engine::render
