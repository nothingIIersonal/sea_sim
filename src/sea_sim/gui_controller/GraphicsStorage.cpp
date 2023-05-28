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

	void GraphicsStorage::drawline(sf::Vector2f a, sf::Vector2f b, float width)
	{
		float lens = pif(b.x - a.x, b.y - a.y);
		float ang = atan2(b.y - a.y, b.x - a.x);
		
		a.x += width / 2.f * sin(ang);
		b.y -= width / 2.f * cos(ang);
		
		sf::RectangleShape line(sf::Vector2f(lens, width));
		
		line.setPosition(a);
		line.setRotation(ang / PI * 180.f);
		line.setFillColor(fill_color_);
		
		parent_ptr_->get_texture(true).draw(line);
	}

	void GraphicsStorage::drawborderline(sf::Vector2f a, sf::Vector2f b, float width)
	{
		float lens = pif(b.x - a.x, b.y - a.y);
		float ang = atan2(b.y - a.y, b.x - a.x);

		a.x += width / 2.f * sin(ang);
		b.y -= width / 2.f * cos(ang);

		sf::RectangleShape line(sf::Vector2f(lens, width));

		line.setPosition(a);
		line.setRotation(ang / PI * 180.f);
		line.setFillColor(outline_color_);

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

		drawborderline(a, b, border_width);
		drawborderline(b, c, border_width);
		drawborderline(c, a, border_width);
	}

	void GraphicsStorage::drawship(const Ship& ship)
	{
		auto angle = ship.get_angle();

		sf::Vector2f position {ship.get_position().x, ship.get_position().y};

		sf::Transform rotation;
		rotation.rotate(angle / PI * 180.f, 0.f, 0.f);

		sf::Vector2f rotator_a = rotation.transformPoint(sf::Vector2f{  30,   0 }) + position;
        sf::Vector2f rotator_b = rotation.transformPoint(sf::Vector2f{ -30, -20 }) + position;
        sf::Vector2f rotator_c = rotation.transformPoint(sf::Vector2f{ -30,  20 }) + position;

		sf::Vector2u view_area = parent_ptr_->get_texture(true).getSize();

		drawtriangle(rotator_a, rotator_b, rotator_c);


		sf::Vector2f min_pos = { min(min(rotator_a.x, rotator_b.x), rotator_c.x), min(min(rotator_a.y, rotator_b.y), rotator_c.y) };
		sf::Vector2f max_pos = { max(max(rotator_a.x, rotator_b.x), rotator_c.x), max(max(rotator_a.y, rotator_b.y), rotator_c.y) };

		sf::Vector2f text_pos =
		{
			max_pos.x + 5,
			min_pos.y + (max_pos.y - min_pos.y) / 2.f
		};

		drawtext(text_pos, ship.get_identifier());
	}

	void GraphicsStorage::drawtext(sf::Vector2f position, const std::string& text)
	{
		const unsigned int character_size = 20;

		sf::Text sf_text;

		sf_text.setPosition(position - sf::Vector2f{0, character_size / 2u});

		sf_text.setFont(parent_ptr_->get_font());
		sf_text.setString(sf::String::fromUtf8(text.begin(), text.end()));
		sf_text.setCharacterSize(character_size);

		sf_text.setFillColor(fill_color_);

		sf_text.setStyle(sf::Text::Bold);

		parent_ptr_->get_texture(true).draw(sf_text);
	}

} // namespace gui
