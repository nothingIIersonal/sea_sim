#pragma once


#include <cstdint>


#include <sea_sim/gears/json/json.hpp>


namespace graphics
{

    struct Color
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

} // namespace graphics


namespace nlohmann
{

    template <>
    struct adl_serializer<graphics::Color> {
        static void to_json(nlohmann::json& j, const graphics::Color& color)
        {
            j = nlohmann::json{ {"r", color.r}, {"g", color.g}, {"b", color.b}, {"a", color.a} };
        }
        static void from_json(const nlohmann::json& j, graphics::Color& color)
        {
            j.at("r").get_to(color.r);
            j.at("g").get_to(color.g);
            j.at("b").get_to(color.b);
            j.at("a").get_to(color.a);
        }
    };

} // namespace nlohmann
