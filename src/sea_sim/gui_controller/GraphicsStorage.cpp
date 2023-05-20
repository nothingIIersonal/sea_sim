#include <sea_sim/gui_controller/GraphicsStorage.h>
#include <sea_sim/gui_controller/RenderEngine.h>

#include <iostream>

namespace gui
{
	using namespace utils;

	GraphicsStorage::GraphicsStorage(RenderEngine* parent_ptr)
		: parent_ptr_(parent_ptr) {}
	GraphicsStorage::~GraphicsStorage()
	{
		parent_ptr_ = nullptr;
	}

	void GraphicsStorage::drawline(sf::Vector2f a, sf::Vector2f b, int width)
	{
		float lens = pif(b.x - a.x, b.y - a.y);
		float ang = atan2(b.y - a.y, b.x - a.x);
		
		a.x += width / 2.f * sin(ang);
		b.y -= width / 2.f * cos(ang);
		
		sf::RectangleShape line(sf::Vector2f(lens, static_cast<float>(width)));
		
		line.setPosition(a);
		line.setRotation(ang / PI * 180.f);
		line.setFillColor(MainColor);
		
		parent_ptr_->get_texture(true).draw(line);
	}

} // namespace gui
