#include <sea_sim/toolkit/object_showcase/object_showcase.h>


namespace nlohmann
{
    void adl_serializer<Ship>::to_json(nlohmann::json& j, const sf::Vector2f& obj)
    {
        j = nlohmann::json{
            {"x", ship.get_x()},
            {"y", ship.get_y()},
            {"staff", ship.get_staff()},
            {"identifier", ship.get_identifier()}
        };
    }
    void adl_serializer<Ship>::from_json(const nlohmann::json& j, Ship& ship)
    {
        ship = Ship{
            j.at("identifier").get<std::string>(),
            j.at("x").get<float>(),
            j.at("y").get<float>(),
            j.at("staff").get<std::vector<std::string>>()
        };
    }
} // namespace nlohmann