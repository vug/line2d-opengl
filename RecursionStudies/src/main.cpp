#include <iostream>
#include <vector>

#include "factorial.h"
#include "fibonacci.h"

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