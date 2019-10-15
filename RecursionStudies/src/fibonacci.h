#pragma once

struct FibonacciFrame {
	int n;
	int stage;
};

int fibonacci(int n);
int fibonacci2(int n);
std::ostream& operator<<(std::ostream& os, const FibonacciFrame& fr);