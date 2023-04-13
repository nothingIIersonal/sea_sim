#pragma once

struct ImVec2;

namespace gui::utils
{
	float min(float a, float b);
	float max(float a, float b);

	int minmax(int a, int num, int b);      // equal to - min(max(num, a), b);
	float minmax(float a, float num, float b);
	bool in(int a, int num, int b);         // true if - a <= num <= b
	bool in_f(float a, float num, float b);
	float mod_f(float X, float Y);          // modulus for float numbers

	double randf(double start, double end, int amount); // returns random float inside (start / end) with the number of values specified in amount

	char const* operator"" _C(const char8_t* str, std::size_t);

	ImVec2 operator+(const ImVec2& left, const ImVec2& right);
	ImVec2 operator-(const ImVec2& left, const ImVec2& right);
	bool operator==(const ImVec2& left, const ImVec2& right);
	bool operator!=(const ImVec2& left, const ImVec2& right);
} // namespace gui::utils