#pragma once

#include<algorithm> 

#include "glsl.h"

vec2::vec2(float x, float y)
	: x(x), y(y) {}

vec2::vec2(float c)
	: x(c), y(c) {}

vec2 vec2::operator+(const vec2& v) {
	return vec2(x + v.x, y + v.y);
}

vec2 vec2::operator-(const vec2& v) {
	return vec2(x - v.x, y - v.y);
}

vec2 vec2::operator*(const float c) {
	return vec2(c * x, c * y);
}


//vec2(float c);

std::ostream& operator<<(std::ostream& os, const vec2& v) {
	os << "{x: " << v.x << ", y: " << v.y << "}";
	return os;
}

/**
 * GLSL math
 */
float length(vec2 v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

float dot(vec2 u, vec2 v) {
	return u.x * v.x + u.y * v.y;
}

float clip(float x, float lo, float hi) {
	if (x < lo) {
		return lo;
	}
	else if (x > hi) {
		return hi;
	}
	return x;
}


/**
 * Shapes
 */
float circleSDF(vec2 p, vec2 c, float r) {
	vec2 d = p - c;
	return length(d) - r;
}

float planeSDF(vec2 p, vec2 pl, vec2 n) {
	return dot(p - pl, n);
}

float segmentSDF(vec2 p, vec2 a, vec2 b) {
	vec2 v = p - a;
	vec2 u = b - a;
	float t = std::max(std::min(dot(u, v) / dot(u, u), 1.0f), 0.0f);
	vec2 d = v - u * t;
	return length(d);
}