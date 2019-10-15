#include <iostream>
#include <vector>

#include "utils.h"

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& stack)
{
	for (auto const& frame : stack) {
		os << frame << std::endl;
	}
	os << stack.size() << std::endl;
	return os;
}