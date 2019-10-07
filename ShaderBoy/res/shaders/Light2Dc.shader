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
#define MAX_STEP 64
#define MAX_DISTANCE 2.0f
#define EPSILON 1e-6f
#define BIAS 1e-6f
#define MAX_DEPTH 3

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
	float reflection;
	float refrection;
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
 // Study shape combinations
Result scene1(vec2 p, vec2 c) {
	Result a = Result(circleSDF(p, vec2(-0.2f, -0.2f), 0.1f), 10.0f, 0.0f, 0.0f);
	Result b = Result(boxSDF(p, vec2(0.5f, 0.5f), 0.0f, vec2(0.3, 0.2f)), 0.0f, 0.2f, 1.5f);
	Result cc = Result(circleSDF(p, c, 0.05f), 20.0f, 0.0f, 0.0f);
	Result d = Result(circleSDF(p, vec2(0.5f, 0.2f), 0.35f), 0.0f, 0.2f, 1.5f);
	Result e = Result(circleSDF(p, vec2(0.5f, 0.8f), 0.35f), 0.0f, 0.2f, 1.5f);
	Result f = Result(boxSDF(p, vec2(0.5f, 0.5f), 0.0f, vec2(0.2, 0.1f)), 0.0f, 0.2f, 1.5f);
	Result g = Result(circleSDF(p, vec2(0.5f, 0.12f), 0.35f), 0.0f, 0.2f, 1.5f);
	Result h = Result(circleSDF(p, vec2(0.5f, 0.87f), 0.35f), 0.0f, 0.2f, 1.5f);
	Result i = Result(circleSDF(p, vec2(0.5f, 0.5f), 0.2f), 0.0f, 0.2f, 1.5f);
	Result j = Result(planeSDF(p, vec2(0.5f, 0.5f), vec2(0.0f, -1.0f)), 0.0f, 0.2f, 1.5f);
	//return unionOp(a, b);
	//return unionOp(cc, intersectOp(d, e));
	return unionOp(cc, subtractOp(f, unionOp(g, h)));
	//return unionOp(cc, intersectOp(i, j));
}


Result scene(vec2 p, vec2 c) {
	return scene1(p, c); // Choose scene
}


/**
 * Rendering
 */
vec2 gradient(vec2 p, vec2 c) {
	vec2 epx = vec2(EPSILON, 0.0f);
	vec2 epy = vec2(0.0f, EPSILON);
	vec2 g = vec2(
		scene(p + epx, c).sd - scene(p - epx, c).sd,
		scene(p + epy, c).sd - scene(p - epy, c).sd
	);
	return normalize(g);
}

vec2 reflect(vec2 i, vec2 n) {
	float d = dot(i, n) * 2.0f;
	return i - n * d;
}

vec3 refract(vec2 i, vec2 n, float eta) {
	float idotn = dot(i, n);
	float k = 1.0f - eta * eta * (1.0f - idotn * idotn);
	if (k < 0.0f)
		return vec3(0.0); // Total internal reflection
	float a = eta * idotn + sqrt(k);
	return vec3(eta * i - a * n, 1.0);
}

float trace(vec2 o, vec2 d, vec2 c) {  // with material
	float t = 0.001f;
	float sign = scene(o, c).sd > 0.0f ? 1.0f : -1.0f;
	float total = 0.0f;
	int depth = 0;
	float prev_ref = 1.0;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		vec2 p = o + d * t;
		Result res = scene(p, c);
		if (res.sd * sign < EPSILON) {
			total += res.emission * prev_ref;
			if (depth < MAX_DEPTH && (res.reflection > 0.0f || res.refrection > 0.0f)) {
				float refl = res.reflection;
				vec2 n = gradient(p, c);
				n *= sign;
				if (res.refrection > 0.0f) {
					refr = refract(d, n, sign < 0.0f ? res.refrection : 1.0f / res.refrection);
					if (refr.z > 0.5) {
						pref_ref = (1.0 - refl);
					}
					else {
						refl = 1.0f; // Total internal reflection
					}
				}
				if (refl > 0.0f) {

				}
				vec2 ref = reflect(d, n);
				o = p + n * BIAS;
				d = normalize(ref);
				depth += 1;
				i = 0;
				t = 0.0f;
				prev_ref = res.reflection;
			}
			else {
				break;
			}
		}
		t += res.sd;
	}
	return total;
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

	//float val = scene(uv, m).sd;
	float val = sample(uv, m);
	color = vec4(vec3(val), 1.0);

	//float n = 50.0f;
	//vec2 uvQ = floor(uv * n) / n;
	//vec2 gQ = gradient(uvQ, m); // quantized gradient
	//vec2 gQR = vec2(-gQ.y, gQ.x);
	//vec2 r = mod(n * uv, 1.0); // cell coordinate
	//float norm = dot(r - vec2(0.5), gQR);
	//color = vec4(norm, val, 0.0, 1.0);
}
