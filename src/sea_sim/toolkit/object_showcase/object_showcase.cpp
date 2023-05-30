#include <sea_sim/toolkit/object_showcase/object_showcase.h>


namespace nlohmann 
{
    void adl_serializer<Ship>::to_json(nlohmann::json& j, const Ship& ship)
    {
        j = nlohmann::json{
            {"identifier", ship.get_identifier()},
            {"position", ship.get_position()},
            {"fill_color", ship.get_fill_color()},
            {"outline_color", ship.get_outline_color()},
            {"angle", ship.get_angle()},
            {"desired_angle", ship.get_desired_angle()},
            {"speed", ship.get_speed()},
            {"max_speed", ship.get_max_speed()},
            {"rotation_speed", ship.get_rotation_speed()},
        };
    }
    void adl_serializer<Ship>::from_json(const nlohmann::json& j, Ship& ship)
    {
        ship = Ship{
            j.at("identifier").get<std::string>(),
            j.at("position").get<geom::Vector2f>(),
            j.at("fill_color").get<graphics::Color>(),
            j.at("outline_color").get<graphics::Color>(),
            j.at("angle").get<float>(),
            j.at("desired_angle").get<float>(),
            j.at("speed").get<float>(),
            j.at("max_speed").get<float>(),
            j.at("rotation_speed").get<float>(),
        };
    }
} // namespace nlohmann