#pragma once
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

		// void insert();
		// void erase();
		// void clear();

		// void render_graphics();

	private:
		RenderEngine* parent_ptr_;

	};
} // namespace gui
