#include <sea_sim/toolkit/object_showcase/object_showcase.h>


namespace nlohmann 
{
    // template <typename T>
    // void adl_serializer<geom::Vector2<T>>::to_json(nlohmann::json& j, const geom::Vector2<T>& vector)
    // {
    //     j = nlohmann::json{
    //         {"x", vector.x},
    //         {"y", vector.y}
    //     };
    // }
    // template <typename T>
    // void adl_serializer<geom::Vector2<T>>::from_json(const nlohmann::json& j, geom::Vector2<T>& vector)
    // {
    //     j.at("x").get_to(vector.x);
    //     j.at("y").get_to(vector.y);
    // }


    void adl_serializer<Ship>::to_json(nlohmann::json& j, Ship& ship)
    {
        j = nlohmann::json{
            {"identifier", ship.get_identifier()},
            {"position", ship.get_position()},
            {"angle", ship.get_angle()}
        };
    }
    void adl_serializer<Ship>::from_json(const nlohmann::json& j, Ship& ship)
    {
        ship = Ship{
            j.at("identifier").get<std::string>(),
            j.at("position").get<geom::Vector2f>(),
            j.at("angle").get<float>()
        };
    }
} // namespace nlohmann