#pragma once
class Vector2
{
public:
	float _x;
	float _y;

	Vector2();
	Vector2(float x, float y);

	Vector2 operator+(const Vector2& other) const; // ∫§≈Õ µ°º¿
	Vector2 operator-(const Vector2& other) const; // ∫§≈Õ ª¨º¿
	bool operator==(const Vector2& other) const; // ∫§≈Õ ∫Ò±≥
	bool operator<(const Vector2& other) const; // ∫§≈Õ ∫Ò±≥

	float Magnitude() const; // ∫§≈Õ ≈©±‚
	Vector2 Normalrize() const; // ¥‹¿ß ∫§≈Õ
	
	float DotProduct(const Vector2& other) const;
	float Cross(const Vector2& other) const;
};
