#include <iostream>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "vendor/stb_image_write.h"

#include "glsl.h"
#include "tests.h"

int main(void)
{
	//test_binary_tree();
	//test_factorial_and_fibonacci();

	const int width = 500, height = 500, channels = 1;
	unsigned char img[width][height];

	//for (int i = 0; i < width; i++) {
	//	for (int j = 0; j < height; j++) {
	//		int y_cell = (i % 50 < 25) ? 1 : -1;
	//		int x_cell = (j % 50 < 25) ? 1 : -1;
	//		int z_cell = x_cell * y_cell == 1 ? 255 : 0;
	//		img[i][j] = z_cell;
	//	}
	//}
	//stbi_write_png("checker.png", width, height, channels, img, width * channels);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			float y = (float)i / height;
			float x = (float)j / width;
			vec2 p = vec2(x, y);
			vec2 center = vec2(0.5, 0.3);
			float d = circleSDF(p, center, 0.05);
			d = abs(d);
			img[i][j] = (int)(clip(d, 0.0f, 1.0f) * 255);
		}
	}
	bool isSuccessful = stbi_write_png("scene.png", width, height, channels, img, width * channels);

	vec2 v = vec2(1.0);
	vec2 u = vec2(0.5, 1.5);
	std::cout 
		<< v << std::endl
		<< u << std::endl
		<< u - v << std::endl
		<< u + v << std::endl
		<< u * 3.0f << std::endl;

	std::cin.ignore();
	return 0;
}