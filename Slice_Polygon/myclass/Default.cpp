#include "Default.hpp"

float random_number(const float& min, const float& max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> urd(min, max);

	return urd(gen);
}

int random_number(const int& min, const int& max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> urd(min, max);

	return urd(gen);
}

void degree_range_normalization(float& degree) {
	while (degree < 0.0f) degree += 360.0f;
	while (360.0f <= degree) degree -= 360.0f;
}