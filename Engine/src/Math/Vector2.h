#pragma once

namespace Engine::math
{

struct Vector2
{
	float X = 0.0f;
	float Y = 0.0f;

	Vector2& operator+=(Vector2 const& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		return *this;
	}
	Vector2& operator-=(Vector2 const& rhs)
	{
		X -= rhs.X;
		Y -= rhs.Y;
		return *this;
	}
	Vector2& operator*=(float scalar)
	{
		X *= scalar;
		Y *= scalar;
		return *this;
	}
};

inline Vector2 operator+(Vector2 lhs, Vector2 const& rhs)
{
	lhs += rhs;
	return lhs;
}

inline Vector2 operator-(Vector2 lhs, Vector2 const& rhs)
{
	lhs -= rhs;
	return lhs;
}

inline Vector2 operator*(float scalar, Vector2 vec)
{
	vec.X *= scalar;
	vec.Y *= scalar;
	return vec;
}

inline Vector2 operator*(Vector2 vec, float scalar) { return scalar * vec; }

inline float Dot(const Vector2& a, const Vector2& b) { return a.X * b.X + a.Y * b.Y; }

struct AABB
{
	Vector2 Min = { 0.0f, 0.0f };
	Vector2 Max = { 0.0f, 0.0f };

	bool Intersects(const AABB& other) const
	{
		if (Max.X < other.Min.X || Min.X > other.Max.X)
			return false;
		if (Max.Y < other.Min.Y || Min.Y > other.Max.Y)
			return false;
		return true;
	}
};

} // namespace Engine::math
