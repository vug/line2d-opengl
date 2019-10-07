#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = position;
	v_TexCoord = texCoord;
}

#shader fragment
#version 330 core

#define TWO_PI 6.28318530718f
#define N 64
#define MAX_STEP 10
#define MAX_DISTANCE 2.0f
#define EPSILON 1e-6f

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform ivec2 u_Resolution;
uniform float u_Time;
uniform vec2 u_Mouse;


/** 
 * Random Functions
 */
float rand(vec2 co) {
	return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash(uint x) {
	x += (x << 10u);
	x ^= (x >> 6u);
	x += (x << 3u);
	x ^= (x >> 11u);
	x += (x << 15u);
	return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash(uvec2 v) { return hash(v.x ^ hash(v.y)); }
uint hash(uvec3 v) { return hash(v.x ^ hash(v.y) ^ hash(v.z)); }
uint hash(uvec4 v) { return hash(v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w)); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct(uint m) {
	const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
	const uint ieeeOne = 0x3F800000u; // 1.0 in IEEE binary32

	m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
	m |= ieeeOne;                          // Add fractional part to 1.0

	float  f = uintBitsToFloat(m);       // Range [1:2]
	return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random(float x) { return floatConstruct(hash(floatBitsToUint(x))); }
float random(vec2  v) { return floatConstruct(hash(floatBitsToUint(v))); }
float random(vec3  v) { return floatConstruct(hash(floatBitsToUint(v))); }
float random(vec4  v) { return floatConstruct(hash(floatBitsToUint(v))); }

/**
 * Shape + Material
 */
struct Result {
	float sd;
	float emission;
};

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
	float t = max(min(dot(u, v) / dot(u, u), 1.0f), 0.0f);
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
	return min(max(dx, dy), 0.0f) + length(a);
}

float triangleSDF(vec2 p, vec2 a, vec2 b, vec2 c) {
	float d = min(min(
		segmentSDF(p, a, b),
		segmentSDF(p, b, c)),
		segmentSDF(p, c, a));
	return (b.x - a.x) * (p.y - a.y) > (b.y - a.y) * (p.x - a.x) &&
		(c.x - b.x) * (p.y - b.y) > (c.y - b.y) * (p.x - b.x) &&
		(a.x - c.x) * (p.y - c.y) > (a.y - c.y) * (p.x - c.x) ? -d : d;
}


/**
 * Scenes
 */
// Study shapes
Result scene3(vec2 p, vec2 c) {
	Result a = Result(circleSDF(vec2(p), c, 0.2f), 1.0f);
	Result b = Result(planeSDF(vec2(p), c, vec2(0.0f, 1.0f)), 0.8f);
	Result cc = Result(capsuleSDF(vec2(p), c, vec2(0.6f, 0.6f), 0.1f), 1.0f);
	Result d = Result(boxSDF(vec2(p), c, TWO_PI / 16.0f + u_Time, vec2(0.3f, 0.1f)), 1.0f);
	Result e = Result(boxSDF(vec2(p), c, TWO_PI / 16.0f + u_Time, vec2(0.3f, 0.1f)) - 0.1f, 1.0f);
	Result f = Result(triangleSDF(vec2(p), c, vec2(0.8f, 0.8f), vec2(0.3f, 0.6f)), 1.0f);
	Result g = Result(triangleSDF(vec2(p), c, vec2(0.8f, 0.8f), vec2(0.3f, 0.6f)) - 0.1f, 1.0f);
	return g;
	//return intersectOp(a, b);
}

// Study multiple emission
Result scene2(vec2 p, vec2 c) {
	Result r1 = Result(circleSDF(p, c, 0.10f), 2.0f);
	Result r2 = Result(circleSDF(p, vec2(0.3f, 0.7f), 0.05f), 0.8f);
	Result r3 = Result(circleSDF(p, vec2(0.7f, 0.5f), 0.10f), 0.0f);
	return unionOp(unionOp(r1, r2), r3);
}

// Study shape combinations
Result scene1(vec2 p, vec2 c) {
	Result a = Result(circleSDF(p, c, 0.20f), 1.0f);
	Result b = Result(circleSDF(p, vec2(0.6f, 0.5f), 0.20f), 0.8f);
	//return unionOp(a, b);
	//return intersectOp(a, b);
	return subtractOp(a, b);
	//return subtractOp(b, a);
	//return complementOp(a);
}


Result scene(vec2 p, vec2 c) {
	return scene2(p, c); // Choose scene
}


/**
 * Rendering
 */
float trace(vec2 o, vec2 d, vec2 c) {  // with material
	float t = 0.0f;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		Result res = scene(o + d * t, c);
		if (res.sd < EPSILON)
			return res.emission;
		t += res.sd;
	}
	return 0.0f;
}

float trace0(vec2 o, vec2 d, vec2 c) {  // just SDF
	float t = 0.0f;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		float sd = circleSDF(o + d * t, c, 0.1f);
		if (sd < EPSILON)
			return 2.0f;
		t += sd;
	}
	return 0.0f;
}

float sample(vec2 p, vec2 c) {
	float total = 0.0f;
	for (int i = 0; i < N; i++) {
		float f = i; // +u_Time * 0.00001;
		//float a = TWO_PI * random(vec3(p, f)); // uniform sampling
		//float a = TWO_PI * i / N; // stratified
		float a = TWO_PI * (i + random(vec3(p, f))) / N; // jittered
		vec2 dir = vec2(cos(a), sin(a));
		total += trace(p, dir, c);
	}
	return total / N;
}

void main()
{
	vec2 uv = vec2(v_TexCoord);
	vec2 m = vec2(u_Mouse) / u_Resolution.x;

	float val = sample(uv, m);
	color = vec4(val, val, val, 1.0);
}
