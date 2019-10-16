#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "binary_tree.h"
#include "utils.h"

void traverse0(Node* nd) {
	if (nd == nullptr) {
		return;
	}
	traverse0(nd->left);
	//std::cout << nd->key << std::endl;
	traverse0(nd->right);
}

void traverse(Node* nd) {
	std::vector<std::string> pre;
	std::vector<std::string> in;
	std::vector<std::string> post;

	std::function<void(Node*)> traverse_helper = [&](Node* nd) -> void {
		if (nd == nullptr) {
			return;
		}
		
		pre.push_back(nd->key);
		traverse_helper(nd->left);
		in.push_back(nd->key);
		traverse_helper(nd->right);
		post.push_back(nd->key);
	};

	traverse_helper(nd);

	for (auto const& item : pre) { std::cout << item; } std::cout << std::endl;
	for (auto const& item : in) { std::cout << item; } std::cout << std::endl;
	for (auto const& item : post) { std::cout << item; } std::cout << std::endl;

	//std::cout << pre; // ? doesn't work.
}

void traverse2(Node* nd) {
	std::vector<TraverseFrame> stack;
	stack.push_back(TraverseFrame{0, nd});
	// ~retval
	std::vector<std::string> pre;
	std::vector<std::string> in;
	std::vector<std::string> post;

	while (stack.size() > 0) {
		TraverseFrame frame = stack.back();
		stack.pop_back();

		switch (frame.stage) {
		case 0: // before calling left child (pre)
			if (frame.node == nullptr) {
				continue;
			}
			pre.push_back(frame.node->key);

			frame.stage += 1;
			stack.push_back(frame);

			stack.push_back(TraverseFrame{ 0, frame.node->left });
			break;
		case 1: // between left and right child (in)
			in.push_back(frame.node->key);

			frame.stage += 1;
			stack.push_back(frame);

			stack.push_back(TraverseFrame{ 0, frame.node->right });
			break;
		case 2: // after calling right child (post)
			post.push_back(frame.node->key);
			break;
		}
	}

	for (auto const& item : pre) { std::cout << item; } std::cout << std::endl;
	for (auto const& item : in) { std::cout << item; } std::cout << std::endl;
	for (auto const& item : post) { std::cout << item; } std::cout << std::endl;
}