#pragma once

struct FactorialFrame {
	int n;
	int stage;
};

int factorial(int n);
int factorial2(int n);

std::ostream& operator<<(std::ostream& os, const FactorialFrame& fr);