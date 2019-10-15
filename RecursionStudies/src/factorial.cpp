#include <iostream>
#include <vector>

#include "factorial.h"

int factorial(int n) {
	// stage-0
	if (n == 0) {
		return 1;
	}
	// return n * factorial(n - 1);
	int tmp1 = factorial(n - 1);
	// stage-1
	return n * tmp1;
}

// Rule 1: have a snapshot struct. 
// a. include arguments that change in recursive calls
// b. local variables that are used after first recursive call
// c. stage indicating "process division"
int factorial2(int n) {
	// Rule 2a: declare return value. set to default.
	int returnValue = 1;

	// Rule 3: create a stack of snapshots
	std::vector<FactorialFrame> stack;

	// Rule 4: push initial function call arguments
	stack.push_back(FactorialFrame{ n, 0 });

	// Rule 5a: loop until stack empties
	while (stack.size() > 0) {
		// Rule 5b: pop at each iteration
		FactorialFrame frame = stack.back();
		stack.pop_back();

		// Rule 6: state machine to keep track of where we are in the a frame's process
		// num_stages = num_recursive_calls + 1
		switch (frame.stage) {
		// Rule 7a: implement processes per division/stage
		// Rule 7a: operations before first recursive call is stage-0
		case 0:
			if (frame.n == 0) {
				// Rule 8: store return values locally
				returnValue = 1;
				// Rule 9 (optional): convert recursive "return" statements into "continue"s
				continue;
			}
			else {
				// Rule 10a: If process to be done after recursive call, first, increment stage and store current frame
				frame.stage += 1;
				stack.push_back(frame);
				// Rule 10b: convert recursive call to iteration by storing a new frame with new call arguments
				stack.push_back(FactorialFrame{ frame.n - 1, 0 });
				continue;  //  9?
			}
			break;
		// Rule 7b: each recursive call increments the stage by 1
		case 1:
			// Rule 8:
			returnValue *= frame.n;
			// Rule 9:
			continue;
			break;
		}
	}

	// Rule 2b: return the "return value"
	return returnValue;
}

std::ostream& operator<<(std::ostream& os, const FactorialFrame& fr)
{
	os << "{n: " << fr.n << ", stage: " << fr.stage << "}";
	return os;
}