#include <iostream>
#include <vector>

int factorial(int n) {
	if (n == 0) {
		return 1;
	}
	return n * factorial(n - 1);
}

struct FactorialFrame {
	int n;
	int stage;
};

std::ostream& operator<<(std::ostream& os, const FactorialFrame& fr)
{
	os << "{n: " << fr.n << ", stage: " << fr.stage << "}";
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& stack)
{
	for (auto const& frame : stack) {
		os << frame << std::endl;
	}
	os << stack.size() << std::endl;
	return os;
}

int factorial2(int n) {
	std::vector<FactorialFrame> stack;
	stack.push_back(FactorialFrame{ n, 0 });
	int retVal = 1;

	while (stack.size() > 0) {
		FactorialFrame frame = stack.back();
		stack.pop_back();
		std::cout << "frame: " << frame << std::endl;

		if (frame.stage == 0) {
			stack.push_back(FactorialFrame{ frame.n, 1 });
			if (frame.n != 1) {
				stack.push_back(FactorialFrame{ frame.n - 1, 0 });
			}
		}
		else {
			retVal *= frame.n;
		}

		std::cout << stack;
	}
	return retVal;
}

struct FibonacciFrame {
	int n;
	int stage;
};

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

int main(void) {
	//int result = factorial2(5);
	for (int i = 0; i < 10; i++) {
		std::cout << fibonacci(i) << ", " << fibonacci2(i) << std::endl;
	}
	//int result = fibonacci(1);
	//std::cout << "result: " << result << std::endl;
	std::cin.ignore();
	return 0;
}