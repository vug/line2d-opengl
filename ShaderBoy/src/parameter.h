#pragma once

#include <map>
#include <optional>
#include <vector>

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

struct SurroundingKeys {
	std::optional<double> left;
	std::optional<double> right;
	WhereInTimeline where;
};

class KeyFramedParameter : Parameter<double> {
private:
	Interpolation m_Interpolation;
	Extrapolation m_Extrapolation;
	SurroundingKeys getSurroundingKeyFrames(double time);
public:
	KeyFramedParameter(Interpolation, Extrapolation);
	KeyFramedParameter(std::map<double, double> keys, Interpolation, Extrapolation);
	std::map<double, double> keyFrames;
	double getValueAt(double time) override;
	void addKeyFrame(double time, double value);
};