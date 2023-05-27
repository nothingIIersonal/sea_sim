#ifndef OBJECT_SHOWCASE
#define OBJECT_SHOWCASE


#include <string>
#include <vector>


#include <sea_sim/gears/json/json.hpp>
#include <sea_sim/toolkit/geom/geom.hpp>


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

    const std::string get_identifier() noexcept { return this->identifier; }
};


/*
* Ship object
*/
class Ship : public Object
{
private:
    geom::Vector2f position;
    float angle;

public:
    Ship() noexcept : Object("unknown") {}
    explicit Ship(const std::string& identifier, geom::Vector2f position, float angle) noexcept : Object(identifier), position(position), angle(angle) {};
    ~Ship() noexcept = default;

    geom::Vector2f get_position() { return this->position; }
    float get_angle() { return this->angle; }

    void set_position(geom::Vector2f position) { this->position = position; }
    void set_angle(float angle) { this->angle = angle; }
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
    template <typename T>
    struct adl_serializer<geom::Vector2<T>> {
        static void to_json(nlohmann::json&, const geom::Vector2<T>&);
        static void from_json(const nlohmann::json&, geom::Vector2<T>&);
    };

    template <>
    struct adl_serializer<Ship> {
        static void to_json(nlohmann::json&, Ship&);
        static void from_json(const nlohmann::json&, Ship&);
    };
} // namespace nlohmann


#endif
