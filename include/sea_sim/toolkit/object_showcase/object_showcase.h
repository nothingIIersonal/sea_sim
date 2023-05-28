#ifndef OBJECT_SHOWCASE
#define OBJECT_SHOWCASE


#include <string>
#include <vector>


#include <sea_sim/gears/json/json.hpp>
#include <sea_sim/toolkit/geom/geom.hpp>
#include <sea_sim/toolkit/graphics/color.hpp>


/*
* Abstract object in vacuum
*/
class Object
{
protected:
    std::string identifier;

public:
    Object() noexcept = delete;
    explicit Object(const std::string& identifier) noexcept : identifier(identifier) {}
    ~Object() noexcept = default;

    std::string get_identifier() const { return this->identifier; }
};


/*
* Ship object
*/
class Ship : public Object
{
private:
    geom::Vector2f position;
    graphics::Color fill_color;
    graphics::Color outline_color;
    float angle;
    float desired_angle;
    float speed;
    float rotation_speed;

public:
    Ship() noexcept : Object("unknown") {}
    explicit Ship(const std::string& identifier, geom::Vector2f position, graphics::Color fill_color,
                  graphics::Color outline_color, float angle, float desired_angle, float speed, float rotation_speed) noexcept
                  : Object(identifier), position(position), fill_color(fill_color), outline_color(outline_color),
                    angle(angle), desired_angle(desired_angle), speed(speed), rotation_speed(rotation_speed) {}
    ~Ship() noexcept = default;

    geom::Vector2f get_position() const { return this->position; }
    graphics::Color get_fill_color() const { return this->fill_color; }
    graphics::Color get_outline_color() const { return this->outline_color; }
    float get_angle() const { return this->angle; }
    float get_desired_angle() const { return this->desired_angle; }
    float get_speed() const { return this->speed; }
    float get_rotation_speed() const { return this->rotation_speed; }

    void set_position(geom::Vector2f position) { this->position = position; }
    void set_fill_color(graphics::Color fill_color) { this->fill_color = fill_color; }
    void set_outline_color(graphics::Color outline_color) { this->outline_color = outline_color; }
    void set_angle(float angle) { this->angle = angle; }
    void set_desired_angle(float desired_angle) { this->desired_angle = desired_angle; }
    void set_speed(float speed) { this->speed = speed; }
    void set_rotatin_speed(float rotatin_speed) { this->rotation_speed = rotatin_speed; }
};


/*
* Island object
*/
class Isle : public Object
{
private:

public:
    Isle() noexcept = default;
    ~Isle() noexcept = default;
};


/*
* Serialization / Deserialization
*/
namespace nlohmann 
{
    template <>
    struct adl_serializer<Ship>
    {
        static void to_json(nlohmann::json& j, const Ship& ship);
        static void from_json(const nlohmann::json& j, Ship& ship);
    };
} // namespace nlohmann


#endif
