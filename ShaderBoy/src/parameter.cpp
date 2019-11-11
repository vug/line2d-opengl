#include "parameter.h"

#include <string>

template<typename T>
ConstantParameter<T>::ConstantParameter(T x) {
	value = x;
}

template<typename T>
T ConstantParameter<T>::getValueAt(double time) {
	return value;
}

template<typename T>
PVector<T>::PVector() {
	m_Dims = std::vector<T>();
}


template<typename T>
PVector<T>::PVector(std::vector<T> dims) : m_Dims(dims) {}

template<typename T>
PVector<T>& PVector<T>::operator+=(PVector<T> const& rhs) {
	bool isSameLength = this->m_Dims.size() == rhs.m_Dims.size();
	if (!isSameLength) {
		throw std::exception("Added PVectors should have the same size.");
	}
	for (int i = 0; i < this->m_Dims.size(); i++) {
		this->m_Dims[i] += rhs.m_Dims[i];
	}
	return *this;
}

template<typename T>
PVector<T>& PVector<T>::operator*=(double x) {
	for (int i = 0; i < this->m_Dims.size(); i++) {
		this->m_Dims[i] *= x;
	}
	return *this;
}

template<typename T>
KeyFramedParameter<T>::KeyFramedParameter(
	Interpolation interpolation,
	Extrapolation extrapolation
) {
	m_Interpolation = interpolation;
	m_Extrapolation = extrapolation;
}

template<typename T>
KeyFramedParameter<T>::KeyFramedParameter(
	std::map<double, T> keys, 
	Interpolation interpolation,
	Extrapolation extrapolation
) : KeyFramedParameter(interpolation, extrapolation) {
	keyFrames = keys;
}

template<typename T>
void KeyFramedParameter<T>::addKeyFrame(double t, T x) {
	keyFrames[t] = x;
}

template<typename T>
SurroundingKeys<T> KeyFramedParameter<T>::getSurroundingKeyFrames(double t) {
	auto firstLaterThanT = keyFrames.lower_bound(t);
	bool isAfterLast = firstLaterThanT == keyFrames.end();
	if (isAfterLast) { // outside
		auto last = std::rbegin(keyFrames);
		return SurroundingKeys<T> { last->first, std::nullopt, WhereInTimeline::after };
	}

	bool isBeforeFirst = firstLaterThanT == keyFrames.begin();
	if (isBeforeFirst) { // outside
		auto first = std::begin(keyFrames);
		return SurroundingKeys<T> { std::nullopt, first->first, WhereInTimeline::before };
	}

	auto right = firstLaterThanT;
	auto left = std::prev(firstLaterThanT);
	return SurroundingKeys<T> { left->first, right->first, WhereInTimeline::in };
}

template<typename T>
T KeyFramedParameter<T>::getValueAt(double t) {
	if (keyFrames.size() == 0) {
		throw std::out_of_range("Received empty map.");
	}
	if (keyFrames.find(t) != keyFrames.end()) {
		return keyFrames[t];
	}
	SurroundingKeys keysAround = getSurroundingKeyFrames(t);
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
template class ConstantParameter<std::string>;
template class ConstantParameter<double>;
template class PVector<int>;
template class PVector<double>;
template class KeyFramedParameter<PVector<int>>;
template class KeyFramedParameter<PVector<double>>;
template class KeyFramedParameter<double>;