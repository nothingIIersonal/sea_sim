#ifndef OBJECT_SHOWCASE
#define OBJECT_SHOWCASE


#include <string>
#include <vector>


/*
* Abstract object in vacuum
*/
class Object
{
private:
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
    float x, y;
    std::vector<std::string> staff;

public:
    Ship() noexcept = default;
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


#endif
