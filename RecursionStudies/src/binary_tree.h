#pragma once

#include <string>

struct Node {
	Node* left;
	Node* right;
	std::string key;
	int value;
};

struct TraverseFrame {
	int stage;
	Node* node;
};

void traverse0(Node* node);
void traverse(Node* node);
void traverse2(Node* node);
