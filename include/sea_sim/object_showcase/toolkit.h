#ifndef TOOLKIT
#define TOOLKIT


#include <cstdint>


/*
* 2-dim. vector
*/
template <typename T>
class Vector2
{
public:
    T x;
    T y;

    Vector2<T>& operator+(const Vector2<T>& v) { this->x += v.x; this->y += v.y; };
    Vector2<T>& operator-(const Vector2<T>& v) { this->x -= v.x; this->y -= v.y; };
    Vector2<T>& operator*(const T n) { this->x *= n; this->y *= n; };
    Vector2<T>& operator/(const T n) { this->x /= n; this->y /= n; };
};

typedef Vector2<int64_t>  Vector2i, Coord2i;
typedef Vector2<uint64_t> Vector2u, Coord2u;
typedef Vector2<float>    Vector2f, Coord2f;
typedef Vector2<double>   Vector2d, Coord2d;


/*
* RGB color
*/
template <typename T>
class Color
{
public:
    uint8_t r, g, b;

    Color& operator+(const Color& c) { this->r += c.r; this->g += c.g; this->b += c.b; };
    Color& operator-(const Color& c) { this->r -= c.r; this->g -= c.g; this->b -= c.b; };
    Color& operator*(const T n) { this->r *= n; this->g *= n; this->b *= n; };
    Color& operator/(const T n) { this->r /= n; this->g /= n; this->b /= n; };
};


#endif