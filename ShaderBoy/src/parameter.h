#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <vector>

#include "boost/operators.hpp"

template<typename T>
class Parameter {
public:
	virtual T getValueAt(double time) = 0;
};

template<typename T>
class ConstantParameter : Parameter<T> {
private:
	T value;
public:
	ConstantParameter(T value);
	T getValueAt(double time) override;
};

// per key-range interpolation will come later
enum class Interpolation { constant, linear };
enum class Extrapolation { constant, loop, pingpong };
enum class WhereInTimeline { before, in, after };

template<typename T>
struct SurroundingKeys {
	std::optional<double> left;
	std::optional<double> right;
	WhereInTimeline where;
};

template<typename T>
class KeyFramedParameter : Parameter<T> {
//static_assert(std::is_arithmetic<T>::value, "T must be arithmetic.");
private:
	Interpolation m_Interpolation;
	Extrapolation m_Extrapolation;
	SurroundingKeys<T> getSurroundingKeyFrames(double time);
public:
	KeyFramedParameter(Interpolation, Extrapolation);
	KeyFramedParameter(std::map<double, T> keys, Interpolation, Extrapolation);
	std::map<double, T> keyFrames;
	T getValueAt(double time) override;
	void addKeyFrame(double time, T value);
};


// Parameter Vector.
template<typename T>
class PVector: boost::addable<PVector<T>>, boost::multipliable<PVector<T>, double> {
public:
	std::vector<T> m_Dims;
	PVector();
	PVector(std::vector<T> dims);
	PVector& operator+=(PVector const& rhs);
	PVector& operator*=(double x);
	friend std::ostream& operator<<(std::ostream& os, const PVector& pv) {
		for (auto item : pv.m_Dims) {
			os << item << " ";
		}
		return os;
	}
};