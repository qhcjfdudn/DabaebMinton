#pragma once
class Vector2
{
public:
	float _x;
	float _y;

	Vector2();
	Vector2(float x, float y);

	Vector2 operator+(const Vector2& other) const;	// º¤ÅÍ µ¡¼À
	Vector2 operator-(const Vector2& other) const;	// º¤ÅÍ »¬¼À
	Vector2 operator*(float scalar) const;			// º¤ÅÍ ½ºÄ®¶ó °ö
	bool operator==(const Vector2& other) const;	// º¤ÅÍ ºñ±³
	bool operator<(const Vector2& other) const;		// º¤ÅÍ ºñ±³
	Vector2& operator+=(const Vector2& other);		// º¤ÅÍ µ¡¼À ´ëÀÔ

	float Magnitude() const; // º¤ÅÍ Å©±â
	Vector2 Normalrize() const; // ´ÜÀ§ º¤ÅÍ
	
	float Dot(const Vector2& other) const;
	float Cross(const Vector2& other) const;
};
