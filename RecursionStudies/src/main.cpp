#include <iostream>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "vendor/stb_image_write.h"

#include "binary_tree.h"
#include "factorial.h"
#include "fibonacci.h"

Node *make_tree()
{
	Node *root = new Node{nullptr, nullptr, "A", 0};
	root->left = new Node{nullptr, nullptr, "B", 0};
	root->right = new Node{nullptr, nullptr, "C", 0};
	root->left->left = new Node{nullptr, nullptr, "D", 0};
	root->left->right = new Node{nullptr, nullptr, "E", 0};
	root->right->left = new Node{nullptr, nullptr, "F", 0};
	root->right->right = new Node{nullptr, nullptr, "G", 0};
	return root;
}

int main(void)
{
	const int width = 500, height = 500, channels = 1;
	unsigned char img[width][height];
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int y = (i % 50 < 25) ? 1 : -1;
			int x = (j % 50 < 25) ? 1 : -1;
			int z = x * y == 1 ? 255 : 0;
			img[i][j] = z;
		}
	}
	bool isSuccessful = stbi_write_png("checker.png", width, height, channels, img, width * channels);

	//Node *root = make_tree();
	//traverse(root);
	//std::cout << std::endl;
	//traverse2(root);

	//for (int i = 1; i < 10; i++) {
	//	std::cout << i << ": "
	//		<< "factorial: " << factorial(i) << ", " << factorial2(i)
	//		<< " fibonacci: " << fibonacci(i) << ", " << fibonacci2(i)
	//		<< std::endl;
	//}

	//std::cin.ignore();
	return 0;
}