#pragma once

#include <sea_sim/gui_controller/functions.h>
#include <sea_sim/toolkit/object_showcase/object_showcase.h>

#include <map>
#include <vector>
#include <string>


namespace gui
{
	class RenderEngine;

	class GraphicsStorage
	{
	public:
		GraphicsStorage(RenderEngine* parent_ptr_);
		~GraphicsStorage();

		void setFillColor(sf::Color color);
		void setOutlineColor(sf::Color color);

		void drawline(sf::Vector2f a, sf::Vector2f b, unsigned int width = 1);
		void drawcircle(sf::Vector2f pos, float radius, float border_width = 1);
		void drawtriangle(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, float border_width = 1);

		void drawship(const Ship& ship);

	private:
		RenderEngine* parent_ptr_;

		sf::Color fill_color_ = { 255, 255, 255 };
		sf::Color outline_color_ = { 255, 255, 255 };
	};
} // namespace gui
