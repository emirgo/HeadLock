#pragma once
#include <numbers>

struct Vector3
{
	constexpr Vector3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f) noexcept :
		x(x), y(y), z(z) { }

	constexpr const Vector3& operator-(const Vector3& other) const noexcept
	{
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}

	constexpr const Vector3& operator+(const Vector3& other) const noexcept
	{
		return Vector3{ x + other.x, y + other.y, z + other.z };
	}

	constexpr const Vector3& operator/(const Vector3& other) const noexcept
	{
		return Vector3{ x / other.x, y / other.y, z / other.z };
	}

	constexpr const Vector3& operator*(const Vector3& other) const noexcept
	{
		return Vector3{ x * other.x, y * other.y, z * other.z };
	}

	float x, y, z;
};

constexpr Vector3 CalculateAngle(const Vector3& local_position,
	const Vector3& enemy_position,
	const Vector3& view_angles) noexcept
{
	return ((enemy_position - local_position).ToAngle() - view_angles);
}