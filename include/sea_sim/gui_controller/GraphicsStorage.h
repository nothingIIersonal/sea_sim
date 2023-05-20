#pragma once
#include <sea_sim/gui_controller/functions.h>

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

		void drawline();

	private:
		RenderEngine* parent_ptr_;

	};
} // namespace gui
