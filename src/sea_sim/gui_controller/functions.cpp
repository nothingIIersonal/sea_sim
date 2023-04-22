#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

#include <SFML/Graphics.hpp>
#include <cmath>

#include <sea_sim/gui_controller/functions.h>


namespace gui::utils
{
	float min(float a, float b) { return (a > b) ? b : a; }
	float max(float a, float b) { return (a > b) ? a : b; }

	int minmax(int a, int num, int b) { return (a > num) ? a : (b < num) ? b : num; }
	float minmax(float a, float num, float b) { return (a > num) ? a : (b < num) ? b : num; }
	bool in(int a, int num, int b) { return (num >= a && num <= b); }
	bool in_f(float a, float num, float b) { return (num >= a - 0.01 && num <= b + 0.01); }
	float mod_f(float X, float Y)
	{
		if (X > 0)
			return fmod(X, Y);
		else
			return fmod(Y - fmod(-X, Y), Y);
	}

	double randf(double start, double end, int amount)
	{
		return start + (rand() % amount) / double(amount) * (end - start);
	}

	char const* operator"" _C(const char8_t* str, std::size_t)
	{
		return reinterpret_cast<const char*>(str);
	}

	ImVec2 operator+(const ImVec2& left, const ImVec2& right)
	{
		return ImVec2(left.x + right.x, left.y + right.y);
	}
	ImVec2 operator-(const ImVec2& left, const ImVec2& right)
	{
		return ImVec2(left.x - right.x, left.y - right.y);
	}
	bool operator==(const ImVec2& left, const ImVec2& right)
	{
		return left.x == right.x && left.y == right.y;
	}
	bool operator!=(const ImVec2& left, const ImVec2& right)
	{
		return left.x != right.x || left.y != right.y;
	}

} // namespace gui::utils
