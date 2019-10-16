#include <iostream>
#include <vector>

#include "fibonacci.h"

std::ostream& operator<<(std::ostream& os, const FibonacciFrame& fr)
{
	os << "{n: " << fr.n << ", stage: " << fr.stage << "}";
	return os;
}

int fibonacci(int n) {
	// stage-0
	if (n == 0) {
		return 0;
	}
	else if (n == 1) {
		return 1;
	}
	else {
		// stage-1/2
		// return fibonacci(n - 1) + fibonacci(n - 2);

		// stage-1
		int tmp1 = fibonacci(n - 1);
		// stage-2
		int tmp2 = fibonacci(n - 2);
		return tmp1 + tmp2;
	}
}

int fibonacci2(int n) {
	std::vector<FibonacciFrame> stack;
	stack.push_back(FibonacciFrame{ 0, n, 0 });
	int returnValue = 0;

	while (stack.size() > 0) {
		FibonacciFrame frame = stack.back();
		stack.pop_back();
		
		switch (frame.stage) {
		case 0: // before calling fib[n-1]
			// stopping condition
			if (frame.n == 0 || frame.n == 1) {
				returnValue = frame.n;
				continue; // a.k.a return
			}
			// store current frame before doing recursive call
			frame.stage += 1;
			stack.push_back(frame);

			// frame of recursive call fib[n-1]
			stack.push_back(FibonacciFrame{ 0, frame.n - 1, 0 });
			break;
		case 1: // after calling fib[n-1]
			// store return value from fib[n-1] in the frame to use it in next stage
			frame.fib_acc = returnValue;

			// store current frame before doing second recursive call
			frame.stage += 1;
			stack.push_back(frame);

			// frame of recursive call fib[n-1]
			stack.push_back(FibonacciFrame{ 0, frame.n - 2, 0 });
			break;
		case 2: // after calling fib[n-2]
			frame.fib_acc += returnValue;  // fib[n-1] + fib[n-2]
			returnValue = frame.fib_acc;
			continue; // a.k.a return
			break;
		}
	}
	return returnValue;
}