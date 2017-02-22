#pragma once
#include <cmath>

class Vector3
{
public:
	Vector3(float x, float y, float z) : _x(x), _y(y), _z(z)
	{}

	inline auto GetX() { return _x; }
	inline auto GetY() { return _y; }
	inline auto GetZ() { return _z; }

	inline Vector3 operator-(const Vector3& other) const
	{
		return Vector3(_x - other._x, _y - other._y, _z - other._z);
	}

	inline float Length()
	{
		return std::sqrt(_x * _x + _y * _y + _z * _z);
	}

	inline float LengthSquared()
	{
		return _x * _x + _y * _y + _z * _z;
	}

private:
    float _x, _y, _z;
};
