#include "parameter.h"

template<typename T>
ConstantParameter<T>::ConstantParameter(T x) {
	value = x;
}

template<typename T>
T ConstantParameter<T>::getValueAt(double time) {
	return value;
}


KeyFramedParameter::KeyFramedParameter() {}

KeyFramedParameter::KeyFramedParameter(std::map<double, double> keys) {
	keyFrames = keys;
}

void KeyFramedParameter::addKeyFrame(double t, double x) {
	keyFrames[t] = x;
}

double KeyFramedParameter::getValueAt(double t) {
	if (keyFrames.size() == 0) {
		throw std::out_of_range("Received empty map.");
	}
	if (keyFrames.find(t) != keyFrames.end()) {
		return keyFrames[t];
	}

	auto low = keyFrames.lower_bound(t);
	if (low == keyFrames.end()) { // outside - higher - extrapolate
		return std::rbegin(keyFrames)->second;
	}
	else if (low == keyFrames.begin()) { // outside - lower - extrapolate
		return low->second;
	}

	// inside - interpolate
	auto prev = std::prev(low);
	// Linear interpolation
	double mix = (t - prev->first) / (low->first - prev->first);
	return (1.0 - mix) * prev->second + mix * low->second;
	// constant interpolation (to closest). TODO: convert this to step function.
	//bool is_prev_closer = t - prev->first < prev->second - t;
	//return is_prev_closer ? prev->second : low->second;
}

// To prevent linker errors
template class ConstantParameter<int>;