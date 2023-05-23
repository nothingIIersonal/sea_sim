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


	void GraphicsStorage::setFillColor(sf::Color color)
	{
		fill_color_ = color;
	}

	void GraphicsStorage::setOutlineColor(sf::Color color)
	{
		outline_color_ = color;
	}

	void GraphicsStorage::drawline(sf::Vector2f a, sf::Vector2f b, unsigned int width)
	{
		float lens = pif(b.x - a.x, b.y - a.y);
		float ang = atan2(b.y - a.y, b.x - a.x);
		
		a.x += width / 2.f * sin(ang);
		b.y -= width / 2.f * cos(ang);
		
		sf::RectangleShape line(sf::Vector2f(lens, static_cast<float>(width)));
		
		line.setPosition(a);
		line.setRotation(ang / PI * 180.f);
		line.setFillColor(fill_color_);
		
		parent_ptr_->get_texture(true).draw(line);
	}

	void GraphicsStorage::drawcircle(sf::Vector2f pos, float radius, float border_width)
	{
		sf::CircleShape circle;

		circle.setPosition(pos.x, pos.y);
		circle.setRadius(radius);

		circle.setOrigin(radius, radius);
		
		circle.setFillColor(sf::Color(fill_color_.r, fill_color_.g, fill_color_.b));

		circle.setOutlineColor(outline_color_);
		circle.setOutlineThickness(border_width);

		parent_ptr_->get_texture(true).draw(circle);
	}

	void GraphicsStorage::drawtriangle(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, float border_width)
	{
		sf::ConvexShape convex;

		convex.setPointCount(3);

		convex.setPoint(0, a);
		convex.setPoint(1, b);
		convex.setPoint(2, c);

		convex.setFillColor(fill_color_);
		convex.setOutlineColor(outline_color_);

		parent_ptr_->get_texture(true).draw(convex);
	}

} // namespace gui
