﻿#include <sea_sim/toolkit/object_showcase/object_showcase.h>


namespace nlohmann 
{
    void adl_serializer<Ship>::to_json(nlohmann::json& j, const Ship& ship)
    {
        j = nlohmann::json{
            {"identifier", ship.get_identifier()},
            {"position", ship.get_position()},
            {"angle", ship.get_angle()},
            {"desired_angle", ship.get_desired_angle()},
            {"speed", ship.get_speed()},
            {"rotation_speed", ship.get_rotation_speed()},
        };
    }
    void adl_serializer<Ship>::from_json(const nlohmann::json& j, Ship& ship)
    {
        ship = Ship{
            j.at("identifier").get<std::string>(),
            j.at("position").get<geom::Vector2f>(),
            j.at("angle").get<float>(),
            j.at("desired_angle").get<float>(),
            j.at("speed").get<float>(),
            j.at("rotation_speed").get<float>(),
        };
    }
} // namespace nlohmann