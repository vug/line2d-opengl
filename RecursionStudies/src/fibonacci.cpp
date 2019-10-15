#include <iostream>
#include <vector>

#include "fibonacci.h"

std::ostream& operator<<(std::ostream& os, const FibonacciFrame& fr)
{
	os << "{n: " << fr.n << ", stage: " << fr.stage << "}";
	return os;
}

int fibonacci(int n) {
	if (n == 0) {
		return 0;
	}
	else if (n == 1) {
		return 1;
	}
	else {
		return fibonacci(n - 1) + fibonacci(n - 2);
	}
}

int fibonacci2(int n) {
	std::vector<FibonacciFrame> stack;
	stack.push_back(FibonacciFrame{ n, 0 });
	int retVal = 0;

	while (stack.size() > 0) {
		FibonacciFrame frame = stack.back();
		stack.pop_back();
		//std::cout << "frame: " << frame << std::endl;
		if (frame.stage == 0) {
			if (frame.n == 0) {
				stack.push_back(FibonacciFrame{ 0, 1 });
			}
			else if (frame.n == 1) {
				stack.push_back(FibonacciFrame{ 1, 1 });
			}
			else {
				stack.push_back(FibonacciFrame{ frame.n - 1, 0 });
				stack.push_back(FibonacciFrame{ frame.n - 2, 0 });
			}
		}
		else {
			retVal += frame.n;
		}
	}
	return retVal;
}