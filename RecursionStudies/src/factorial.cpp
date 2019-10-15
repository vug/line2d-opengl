#include <iostream>
#include <vector>

#include "factorial.h"

int factorial(int n) {
	if (n == 0) {
		return 1;
	}
	return n * factorial(n - 1);
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
	}
	return retVal;
}

std::ostream& operator<<(std::ostream& os, const FactorialFrame& fr)
{
	os << "{n: " << fr.n << ", stage: " << fr.stage << "}";
	return os;
}