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

vec2 max(const vec2& u, const vec2& v) {
	return vec2(std::max(u.x, v.x), std::max(u.y, v.y));
}

vec2 normalize(vec2 v) {
	return v * (1.0f / length(v));
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

float capsuleSDF(vec2 p, vec2 a, vec2 b, float r) {
	return segmentSDF(p, a, b) - r;
}

float boxSDF(vec2 p, vec2 c, float th, vec2 s) {
	float costh = cos(th);
	float sinth = sin(th);
	float dx = abs((p.x - c.x) * costh + (p.y - c.y) * sinth) - s.x;
	float dy = abs((p.y - c.y) * costh - (p.x - c.x) * sinth) - s.y;
	vec2 a = max(vec2(dx, dy), vec2(0.0f));
	return std::min(std::max(dx, dy), 0.0f) + length(a);
}

float triangleSDF(vec2 p, vec2 a, vec2 b, vec2 c) {
	float d = std::min(std::min(
		segmentSDF(p, a, b),
		segmentSDF(p, b, c)),
		segmentSDF(p, c, a));
	return (b.x - a.x) * (p.y - a.y) > (b.y - a.y) * (p.x - a.x) &&
		(c.x - b.x) * (p.y - b.y) > (c.y - b.y) * (p.x - b.x) &&
		(a.x - c.x) * (p.y - c.y) > (a.y - c.y) * (p.x - c.x) ? -d : d;
}

/**
 * Shape Combinations
 */
Result unionOp(Result a, Result b) {
	return a.sd < b.sd ? a : b;
}

Result intersectOp(Result a, Result b) {
	Result r = a.sd > b.sd ? b : a;
	r.sd = a.sd > b.sd ? a.sd : b.sd;
	return r;
}

Result subtractOp(Result a, Result b) {
	Result r = a;
	r.sd = (a.sd > -b.sd) ? a.sd : -b.sd;
	return r;
}

Result complementOp(Result a) {
	a.sd = -a.sd;
	return a;
}


/**
 * Rendering
 */
vec2 reflect(vec2 i, vec2 n) {
	float d = dot(i, n) * 2.0f;
	return i - n * d;
}

Refraction refract(vec2 i, vec2 n, float eta) {
	float idotn = dot(i, n);
	float k = 1.0f - eta * eta * (1.0f - idotn * idotn);
	if (k < 0.0f)
		return Refraction { vec2(0.0f), true }; // Total internal reflection
	float a = eta * idotn + sqrt(k);
	return Refraction{ i * eta - n * a, false };
}
