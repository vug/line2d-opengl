#include "parameter.h"

template<typename T>
ConstantParameter<T>::ConstantParameter(T x) {
	value = x;
}

template<typename T>
T ConstantParameter<T>::getValueAt(double time) {
	return value;
}


KeyFramedParameter::KeyFramedParameter(
	Interpolation interpolation,
	Extrapolation extrapolation
) {
	m_Interpolation = interpolation;
	m_Extrapolation = extrapolation;
}

KeyFramedParameter::KeyFramedParameter(
	std::map<double, double> keys, 
	Interpolation interpolation,
	Extrapolation extrapolation
) : KeyFramedParameter(interpolation, extrapolation) {
	keyFrames = keys;
}

void KeyFramedParameter::addKeyFrame(double t, double x) {
	keyFrames[t] = x;
}

SurroundingKeys KeyFramedParameter::getSurroundingKeyFrames(double t) {
	auto firstLaterThanT = keyFrames.lower_bound(t);
	bool isAfterLast = firstLaterThanT == keyFrames.end();
	if (isAfterLast) { // outside
		auto last = std::rbegin(keyFrames);
		return SurroundingKeys { last->first, std::nullopt, WhereInTimeline::after };
	}

	bool isBeforeFirst = firstLaterThanT == keyFrames.begin();
	if (isBeforeFirst) { // outside
		auto first = std::begin(keyFrames);
		return SurroundingKeys{ std::nullopt, first->first, WhereInTimeline::before };
	}

	auto right = firstLaterThanT;
	auto left = std::prev(firstLaterThanT);
	return SurroundingKeys{ left->first, right->first, WhereInTimeline::in };
}

double KeyFramedParameter::getValueAt(double t) {
	if (keyFrames.size() == 0) {
		throw std::out_of_range("Received empty map.");
	}
	if (keyFrames.find(t) != keyFrames.end()) {
		return keyFrames[t];
	}
	auto keysAround = getSurroundingKeyFrames(t);
	double left = keysAround.left.value_or(-1.0);
	double right = keysAround.right.value_or(-1.0);
	switch (keysAround.where) {
		case WhereInTimeline::before:
			switch (m_Extrapolation) {
			case Extrapolation::constant:
				return keyFrames[right];
				break;
			}
			break;
		case WhereInTimeline::after:
			switch (m_Extrapolation) {
			case Extrapolation::constant:
				return keyFrames[left];
				break;
			}
			break;
		case WhereInTimeline::in:
			switch (m_Interpolation) {
			case Interpolation::constant:
				return keyFrames[left];
				break;
			case Interpolation::linear:
				auto leftVal = keyFrames[left];
				auto rightVal = keyFrames[right];
				double mix = (t - left) / (right - left);
				return (1.0 - mix) * leftVal + mix * rightVal;
				break;
			}
			break;
	}
}

// To prevent linker errors
template class ConstantParameter<int>;