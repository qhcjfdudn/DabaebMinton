#include "ServerPCH.h"
#include "Vector2.h"

Vector2::Vector2() : _x(0), _y(0) {}
Vector2::Vector2(float x, float y) : _x(x), _y(y) {}

Vector2 Vector2::operator+(const Vector2& other) const
{
	float x = _x + other._x;
	float y = _y + other._y;

	return Vector2(x, y);
}
Vector2 Vector2::operator-(const Vector2& other) const
{
	float x = _x - other._x;
	float y = _y - other._y;

	return Vector2(x, y);
}
bool Vector2::operator==(const Vector2& other) const
{
	return _x == other._x && _y == other._y;
}
bool Vector2::operator<(const Vector2& other) const
{
	if (_x == other._x)
		return _y < other._y;

	return _x < other._x;
}

float Vector2::Magnitude() const
{
	return static_cast<float>(sqrt(pow(_x, 2) + pow(_y, 2)));
}
Vector2 Vector2::Normalrize() const
{
	float magnitude = Magnitude();
	return { _x / magnitude, _y / magnitude };
}

float Vector2::Dot(const Vector2& other) const
{
	return _x * other._x + _y * other._y;
}
float Vector2::Cross(const Vector2& other) const
{
	return _x * other._y - _y * other._x;
}