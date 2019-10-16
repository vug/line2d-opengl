#pragma once

struct FibonacciFrame {
	// phase in recursive calls
	int stage;
	// input arguments
	int n;
	// intermediate values
	int fib_acc = 0;
};

int fibonacci(int n);
int fibonacci2(int n);
std::ostream& operator<<(std::ostream& os, const FibonacciFrame& fr);