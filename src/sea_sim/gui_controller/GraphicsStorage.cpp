#include <sea_sim/gui_controller/GraphicsStorage.h>

namespace gui
{
	GraphicsStorage::GraphicsStorage(RenderEngine* parent_ptr)
		: parent_ptr_(parent_ptr) {}
	GraphicsStorage::~GraphicsStorage()
	{
		parent_ptr_ = nullptr;
	}

} // namespace gui
