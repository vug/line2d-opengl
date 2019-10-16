#include <iostream>
#include <vector>

#include "utils.h"

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vector)
{
	for (auto const& item : vector) {
		os << item << std::endl;
	}
	os << vector.size() << std::endl;
	return os;
}