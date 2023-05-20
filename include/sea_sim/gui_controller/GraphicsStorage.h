#pragma once

#include <sea_sim/gui_controller/functions.h>

#include <map>
#include <vector>
#include <string>

struct PosF;


namespace gui
{
	class RenderEngine;

	class GraphicsStorage
	{
	public:
		GraphicsStorage(RenderEngine* parent_ptr_);
		~GraphicsStorage();

		void drawline(sf::Vector2f a, sf::Vector2f b, int width = 1);

	private:
		RenderEngine* parent_ptr_;

		sf::Color MainColor = { 255, 255, 255 };
	};
} // namespace gui
