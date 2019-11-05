#pragma once

#include <map>
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


class KeyFramedParameter : Parameter<double> {
private:
	std::map<double, double> keyFrames;
public:
	KeyFramedParameter();
	KeyFramedParameter(std::map<double, double> keys);
	double getValueAt(double time) override;
	void addKeyFrame(double time, double value);
	//void getClosestKeyFrame(double time);
};