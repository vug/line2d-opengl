#pragma once

#include <ostream>
#include <cmath>

/**
 * Vectors
 */
class vec2 {
public:
	vec2(float x, float y);
	vec2(float c);
	float x, y;
	vec2 operator+(const vec2& u);
	vec2 operator-(const vec2& u);
	vec2 operator*(const float c);
private:
	
};

std::ostream& operator<<(std::ostream& os, const vec2& v);

float length(vec2 v);
float dot(vec2 u, vec2 v);
float clip(float x, float lo, float hi);

/**
 * Shapes
 */
float circleSDF(vec2 p, vec2 c, float r);