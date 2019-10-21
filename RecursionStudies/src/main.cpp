#include <ctime>
#include <iostream>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "vendor/stb_image_write.h"

#include "glsl.h"
#include "tests.h"

/**
 * Scenes
 */
float scene0(vec2 p) {
	vec2 center = vec2(0.1f, 0.1f);
	float d;
	//d = circleSDF(p, center, 0.05);
	//d = planeSDF(p, vec2(0.75, 0.75), vec2(-0.25, 0.75));
	//d = segmentSDF(p, vec2(0.25), vec2(0.75));
	//d = capsuleSDF(p, vec2(0.25), vec2(0.75), 0.2);
	//d = boxSDF(p, vec2(0.25f), 2.0f, vec2(0.4f, 0.2f));
	d = triangleSDF(p, vec2(0.25f), vec2(0.75f), vec2(0.1f, 0.9f));
	//d = abs(d);
	return d;
}

Result scene1(vec2 p, vec2 c) {
	float u_ReflectionCoef = 0.5;
	Result a = Result{ circleSDF(p, c, 0.1f), 2.0f, 0.0f };
	Result b = Result{ boxSDF(p, vec2(0.5f, 0.8f), TWO_PI / 16.0f, vec2(0.1f, 0.1f)), 0.0f, 0.9f };
	Result cc = Result{ boxSDF(p, vec2(0.8f, 0.5f), TWO_PI / 16.0f, vec2(0.1f, 0.1f)), 0.0f, 0.9f };
	Result d = Result{ planeSDF(p, vec2(0.0f, 0.5f), vec2(0.0f, -1.0f)), 0.0f, u_ReflectionCoef };
	Result e = Result{ circleSDF(p, vec2(0.5f, 0.5f), 0.4f), 0.0f, u_ReflectionCoef };
	//return unionOp(unionOp(a, b), cc);
	return unionOp(a, subtractOp(d, e));
	//return unionOp(a, d);
}

Result scene2(vec2 p, vec2 c) {
	Result a = Result{ circleSDF(p, c, 0.1f), 2.0f, 0.0f, 0.0f };
	Result b = Result{ boxSDF(p, vec2(0.5f, 0.5f), 0.0f, vec2(0.3f, 0.2f)), 0.0f, 0.5f, 1.5f };
	Result cc = Result{ circleSDF(p, c, 0.05f), 2.0f, 0.0f, 0.0f };
	Result d = Result{circleSDF(p, vec2(0.5f, 0.2f), 0.35f), 0.0f, 0.2f, 1.5f};
	Result e = Result{circleSDF(p, vec2(0.5f, 0.8f), 0.35f), 0.0f, 0.2f, 1.5f};
	Result f = Result{boxSDF(p, vec2(0.5f, 0.5f), 0.0f, vec2(0.2f, 0.1f)), 0.0f, 0.2f, 1.5f};
	Result g = Result{circleSDF(p, vec2(0.5f, 0.12f), 0.35f), 0.0f, 0.2f, 1.5f};
	Result h = Result{circleSDF(p, vec2(0.5f, 0.87f), 0.35f), 0.0f, 0.2f, 1.5f};
	Result i = Result{circleSDF(p, vec2(0.5f, 0.5f), 0.2f), 0.0f, 0.2f, 1.5f};
	Result j = Result{planeSDF(p, vec2(0.5f, 0.5f), vec2(0.0f, -1.0f)), 0.0f, 0.2f, 1.5f};
	//return a;
	//return unionOp(a, b);
	return unionOp(cc, intersectOp(d, e));
	//return unionOp(cc, subtractOp(f, unionOp(g, h)));
}

Result scene(vec2 p, vec2 c) {
	return scene2(p, c);
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

float trace1(vec2 o, vec2 d, vec2 c) {
	float t = 0.0f;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		Result res = scene(o + d * t, c);
		if (res.sd < EPSILON)
			return res.emission;
		t += res.sd;
	}
	return 0.0f;
}

float trace2(vec2 o, vec2 d, vec2 c, int depth) {
	float t = 0.0f;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		vec2 p = o + d * t;
		Result res = scene(p, c);
		if (res.sd < EPSILON) {
			float sum = res.emission;
			if (depth < MAX_DEPTH && res.reflection > 0.0f) {
				vec2 n = gradient(p, c);
				vec2 r = reflect(d, n);
				sum += res.reflection * trace2(p + n * BIAS, r, c, depth + 1);
			}
			return sum;
		}
		t += res.sd;
	}
	return 0.0f;
}

float trace2b(vec2 o, vec2 d, vec2 c, int depth) {
	float t = 0.0f;
	bool didHit = false;
	Result res;
	vec2 p = vec2(0.0f);
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		p = o + d * t;
		res = scene(p, c);
		if (res.sd < EPSILON) {
			didHit = true;
			break;
		}
		t += res.sd;
	}
	if (!didHit) {
		return 0.0f;
	}
	float sum = res.emission;
	if (depth >= MAX_DEPTH || res.reflection <= 0.0f) {
		return sum;
	}
	vec2 n = gradient(p, c);
	vec2 r = reflect(d, n);
	sum += res.reflection * trace2b(p + n * BIAS, r, c, depth + 1);
	return sum;
}

float trace2c(vec2 o, vec2 d, vec2 c, int depth, float acc_emm = 0.0, float acc_ref = 1.0f) {
	float t = 0.0f;
	bool didHit = false;
	Result res;
	vec2 p = vec2(0.0f);
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		p = o + d * t;
		res = scene(p, c);
		if (res.sd < EPSILON) {
			didHit = true;
			break;
		}
		t += res.sd;
	}
	if (!didHit) {
		return acc_emm;
	}
	acc_emm += acc_ref * res.emission;
	if (depth >= MAX_DEPTH || res.reflection <= 0.0f) {
		return acc_emm;
	}
	vec2 n = gradient(p, c);
	vec2 r = reflect(d, n);
	acc_ref *= res.reflection;
	return trace2c(p + n * BIAS, r, c, depth + 1, acc_emm, acc_ref);
}

float trace2d(vec2 o, vec2 d, vec2 c) {
	float t = 0.0f;
	bool didHit = false;
	int depth = 0;
	float acc_emm = 0.0;
	float acc_ref = 1.0f;
	Result res;
	vec2 p = vec2(0.0f);

	while (true) {
		for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
			p = o + d * t;
			res = scene(p, c);
			if (res.sd < EPSILON) {
				didHit = true;
				break;
			}
			t += res.sd;
		}
		if (!didHit) {
			break;
		}
		acc_emm += acc_ref * res.emission;
		if (depth >= MAX_DEPTH || res.reflection <= 0.0f) {
			break;
		}
		vec2 n = gradient(p, c);
		vec2 r = reflect(d, n);
		
		o = p + n * BIAS;
		d = r;
		c = c;
		depth = depth + 1;
		acc_emm = acc_emm;
		acc_ref *= res.reflection; 
		
		t = 0.0f;
		didHit = false;
	}
	return acc_emm;
}



float trace3(vec2 o, vec2 d, vec2 c, int depth) {
	float t = 1e-3f;
	float sign = scene(o, c).sd > 0.0f ? 1.0f : -1.0f;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		vec2 p = o + d * t;
		Result res = scene(p, c);
		if (res.sd * sign < EPSILON) {
			float sum = res.emission;
			if (depth < MAX_DEPTH && (res.reflection > 0.0f || res.refraction > 0.0f)) {
				float refl = res.reflection;
				vec2 n = gradient(p, c) * sign;
				if (res.refraction > 0.0f) {
					float eta = sign < 0.0f ? res.refraction : 1.0f / res.refraction;
					Refraction rf = refract(d, n, eta);
					if (rf.isTotalInternal) {
						refl = 1.0f;
					}
					else {
						sum += (1.0f - refl) * trace3(p - n * BIAS, rf.dir, c, depth + 1);
					}
				}
				if (refl > 0.0f) {
					vec2 rl = reflect(d, n);
					sum += refl * trace3(p + n * BIAS, rl, c, depth + 1);
				}
			}
			return sum;
		}
		t += res.sd * sign;
	}
	return 0.0f;
}



float trace3b(vec2 o, vec2 d, vec2 c, int depth) {
	float t = 1e-3f;
	bool didHit = false;
	Result res;
	vec2 p = vec2(0.0f);
	float sign = scene(o, c).sd > 0.0f ? 1.0f : -1.0f;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		p = o + d * t;
		res = scene(p, c);
		if (res.sd * sign < EPSILON) {
			didHit = true;
			break;
		}
		t += res.sd * sign;
	}
	if (!didHit) {
		return 0.0f;
	}

	float sum = res.emission;
	if (depth >= MAX_DEPTH || (res.reflection <= 0.0f && res.refraction <= 0.0f)) {
		return sum;
	}
	float refl = res.reflection;
	vec2 n = gradient(p, c) * sign;
	if (res.refraction > 0.0f) {
		float eta = sign < 0.0f ? res.refraction : 1.0f / res.refraction;
		Refraction rf = refract(d, n, eta);
		if (rf.isTotalInternal) {
			refl = 1.0f;
		}
		else {
			sum += (1.0f - refl) * trace3b(p - n * BIAS, rf.dir, c, depth + 1);
		}
	}
	if (refl > 0.0f) {
		vec2 rl = reflect(d, n);
		sum += refl * trace3b(p + n * BIAS, rl, c, depth + 1);
	}
	return sum;
}


float trace3c(vec2 o, vec2 d, vec2 c, int depth, float acc_emm = 0.0, float acc_ref = 1.0f) {
	float t = 1e-3f;
	bool didHit = false;
	Result res;
	vec2 p = vec2(0.0f);
	float sign = scene(o, c).sd > 0.0f ? 1.0f : -1.0f;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		p = o + d * t;
		res = scene(p, c);
		if (res.sd * sign < EPSILON) {
			didHit = true;
			break;
		}
		t += res.sd * sign;
	}
	if (!didHit) {
		return acc_emm;
	}
	acc_emm += acc_ref * res.emission;
	if (depth >= MAX_DEPTH || (res.reflection <= 0.0f && res.refraction <= 0.0f)) {
		return acc_emm;
	}
	float refl = res.reflection;
	vec2 n = gradient(p, c) * sign;
	float tot = 0.0f;
	if (res.refraction > 0.0f) {
		float eta = sign < 0.0f ? res.refraction : 1.0f / res.refraction;
		Refraction rf = refract(d, n, eta);
		if (rf.isTotalInternal) {
			refl = 1.0f;
		}
		else {
			acc_ref *= (1.0f - refl);
			tot += trace3c(p - n * BIAS, rf.dir, c, depth + 1, acc_emm, acc_ref);
		}
	}
	if (refl > 0.0f) {
		vec2 rl = reflect(d, n);
		acc_ref *= refl;
		tot += trace3c(p + n * BIAS, rl, c, depth + 1, acc_emm, acc_ref);
	}
	return tot;
}


float sample(vec2 p, vec2 c, bool isTest = false) {
	float total = 0.0f;
	for (int i = 0; i < N; i++) {
		float f = i; // +u_Time * 0.00001;
		//float a = TWO_PI * (float)std::rand() / RAND_MAX; // uniform sampling
		//float a = TWO_PI * i / N; // stratified
		float a = TWO_PI * (i + (float)std::rand() / RAND_MAX) / N; // jittered
		vec2 dir = vec2(cos(a), sin(a));
		if (isTest) {
			//total += trace1b(p, dir, c);

			//total += trace2b(p, dir, c, 0); // nicer-recursive
			//total += trace2c(p, dir, c, 0); // tail-recursive
			//total += trace2d(p, dir, c); // iterative!

			//total += trace3b(p, dir, c, 0); // nicer-recursive
			total += trace3c(p, dir, c, 0); // tail-recursive (?)
		}
		else {
			//total += trace1(p, dir, c);

			//total += trace2(p, dir, c, 0); // recursive

			total += trace3(p, dir, c, 0);
		}
	}
	return total / N;
}


int main(void)
{
	//test_binary_tree();
	//test_factorial_and_fibonacci();
	//test_image_write();
	std::srand(std::time(nullptr));

	const int width = 100, height = 100, channels = 1;
	unsigned char img[width][height];
	vec2 c = vec2(0.4, 0.25);

	vec2 coordinates[] = { vec2(0.5, 0.5), vec2(0.5, 0.25), vec2(0.6, 0.7) };
	for (auto p : coordinates) {
		float val = sample(p, c);
		std::cout << "p: " << p << ", val: " << val << std::endl;
		val = sample(p, c, true);
		std::cout << "p: " << p << ", val: " << val << " (test)" << std::endl;
	}

	//for (int i = 0; i < width; i++) {
	//	for (int j = 0; j < height; j++) {
	//		float y = (float)i / height;
	//		float x = (float)j / width;
	//		vec2 p = vec2(x, y);
	//		float val = sample(p, c);
	//		img[i][j] = (int)(val * 255);
	//	}
	//}
	//bool isSuccessful = stbi_write_png("scene.png", width, height, channels, img, width * channels);

	//const int width = 500, height = 500, channels = 4;
	//unsigned char img[width][height][channels];
	//vec2 c = vec2(0.1, 0.1);

	//for (int i = 0; i < width; i++) {
	//	for (int j = 0; j < height; j++) {
	//		float y = (float)i / height;
	//		float x = (float)j / width;
	//		vec2 p = vec2(x, y);
	//		Result res = scene(p, c);
	//		float val = res.sd;
	//		int r = (int)(val >= 0 ? val * 255 : 0);
	//		int g = 0;
	//		int b = (int)(val < 0 ? -val * 255 : 0);
	//		img[i][j][0] = r;
	//		img[i][j][1] = g;
	//		img[i][j][2] = b;
	//		img[i][j][3] = 255;
	//	}
	//}
	//bool isSuccessful = stbi_write_png("scene.png", width, height, channels, img, width * channels);

	//vec2 v = vec2(1.0);
	//vec2 u = vec2(0.5, 1.5);
	//std::cout 
	//	<< v << std::endl
	//	<< u << std::endl
	//	<< u - v << std::endl
	//	<< u + v << std::endl
	//	<< u * 3.0f << std::endl;

	std::cin.ignore();
	return 0;
}