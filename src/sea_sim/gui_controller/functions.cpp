#include <iostream>

#include <sea_sim/gui_controller/functions.h>

namespace gui::utils
{
	// --- Math

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

	// --- ImGui

	inline namespace literals
	{
		char const* operator"" _C(const char8_t* str, std::size_t)
		{
			return reinterpret_cast<const char*>(str);
		}
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

	bool block()
	{
		ImGui::SameLine();
		return true;
	}
	void SelectableColor(ImU32 color)
	{
		ImVec2 p_min = ImGui::GetItemRectMin();
		ImVec2 p_max = ImGui::GetItemRectMax();
		ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
	}
	float get_button_width(std::string text, ImGuiStyle& style)
	{
		return ImGui::CalcTextSize(text.c_str()).x + style.FramePadding.x * 2 + style.ItemSpacing.x;
	}

	// --- SFML

	bool keyHit[sf::Keyboard::KeyCount] = { 0 };

	int mouse_down(const sf::Mouse::Button& B)
	{
		return sf::Mouse::isButtonPressed(B);
	}
	int key_down(const sf::Keyboard::Key& B)
	{
		return sf::Keyboard::isKeyPressed(B);
	}
	int key_hit(const sf::Keyboard::Key& key)
	{
		if (!keyHit[key]) {
			if (sf::Keyboard::isKeyPressed(key)) {
				keyHit[key] = true;
				return true;
			}
		}
		return false;
	}

} // namespace gui::utils
