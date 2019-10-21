#pragma once

#include <ostream>
#include <cmath>

#define EPSILON 1e-6f
#define TWO_PI 6.28318530718f

#define N 1024
#define MAX_STEP 128
#define MAX_DISTANCE 2.0f

#define MAX_DEPTH 5
#define BIAS 1e-6f

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
vec2 max(const vec2& u, const vec2& v);

float length(vec2 v);
float dot(vec2 u, vec2 v);
float clip(float x, float lo, float hi);
vec2 normalize(vec2 v);

/**
 * Shape + Material
 */
struct Result {
	float sd;
	float emission;
	float reflection;
	float refraction;
};

struct Refraction {
	vec2 dir;
	bool isTotalInternal;
};

/**
 * Shapes
 */
float circleSDF(vec2 p, vec2 c, float r);
float planeSDF(vec2 p, vec2 pl, vec2 n);
float segmentSDF(vec2 p, vec2 a, vec2 b);
float capsuleSDF(vec2 p, vec2 a, vec2 b, float r);
float boxSDF(vec2 p, vec2 c, float th, vec2 s);
float triangleSDF(vec2 p, vec2 a, vec2 b, vec2 c);

/**
 * Shape Combinations
 */
Result unionOp(Result a, Result b);
Result intersectOp(Result a, Result b);
Result subtractOp(Result a, Result b);
Result complementOp(Result a);

/**
 * Rendering
 */
vec2 reflect(vec2 i, vec2 n);
Refraction refract(vec2 i, vec2 n, float eta);