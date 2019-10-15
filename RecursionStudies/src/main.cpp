#include <iostream>
#include <vector>

#include "factorial.h"
#include "fibonacci.h"

int main(void) {
	for (int i = 1; i < 10; i++) {
		std::cout << i << ": " 
			<< "factorial: " << factorial(i) << ", " << factorial2(i)
			<< " fibonacci: " << fibonacci(i) << ", " << fibonacci2(i)
			<< std::endl;
	}
	//int result = fibonacci(1);
	//std::cout << "result: " << result << std::endl;
	std::cin.ignore();
	return 0;
}