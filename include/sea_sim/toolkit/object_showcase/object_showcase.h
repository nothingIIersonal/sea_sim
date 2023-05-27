#ifndef OBJECT_SHOWCASE
#define OBJECT_SHOWCASE

#include <sea_sim/gears/json/json.hpp>

#include <string>
#include <vector>


/*
* Abstract object in vacuum
*/
class Object
{
protected:
    std::string identifier;

public:
    Object(const std::string& identifier) noexcept : identifier(identifier) {}
    const std::string get_identifier() noexcept { return this->identifier; }
};


/*
* Ship object
*/
class Ship : public Object
{
private:
    float x = 0, y = 0;
    std::vector<std::string> staff;

public:
    Ship() noexcept : Object("unnamed") {};
    Ship(const std::string& identifier, float x, float y, const std::vector<std::string>& staff) noexcept : Object(identifier), x(x), y(y), staff(staff) {};
    ~Ship() noexcept = default;

    std::vector<std::string> get_staff() { return this->staff; };
    float get_x() { return this->x; }
    float get_y() { return this->y; }
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

namespace nlohmann 
{
    template <>
    struct adl_serializer<Ship>
    {
        static void to_json(nlohmann::json& j, Ship& ship);
        static void from_json(const nlohmann::json& j, Ship& ship);
    };
} // namespace nlohmann

#endif
