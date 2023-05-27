#pragma once

#include <tuple>

namespace geom {

template <typename T>
struct Vector2 {
public:
    using value_type = T;
    using const_value_type = const T;
public:
    constexpr Vector2<T>& operator+=(const Vector2<T>& other);
    constexpr Vector2<T>& operator-=(const Vector2<T>& other);
    constexpr Vector2<T>& operator*=(const T& val);
    constexpr Vector2<T>& operator/=(const T& val);
    constexpr Vector2<T>& operator%=(const T& val);
public:
    T x, y;
};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2u = Vector2<std::uint32_t>;
using Vector2d = Vector2<double>;

template<typename T>
using vec2_ptr = Vector2<T>*;

using vec2i_ptr = vec2_ptr<int>;
using vec2f_ptr = vec2_ptr<float>;
using vec2u_ptr = vec2_ptr<std::uint32_t>;
using vec2d_ptr = vec2_ptr<double>;

///////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& other) {
    x += other.x;
    y += other.y;
    return *this;
}
template <typename T>
constexpr Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator*=(const T& value) {
    x *= value;
    y *= value;
    return *this;
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator/=(const T& value) {
    x /= value;
    y /= value;
    return *this;
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator%=(const T& value) {
    x %= value;
    y %= value;
    return *this;
}

template <typename T>
constexpr bool operator==(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y);
}

template <typename T>
constexpr bool operator!=(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return !(lhs == rhs);
}

template <typename T>
constexpr Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    Vector2<T> temp = lhs;
    temp += rhs;
    return temp;
}

template <typename T>
constexpr Vector2<T> operator-(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    Vector2<T> temp = lhs;
    temp -= rhs;
    return temp;
}

template <typename T, typename U>
constexpr Vector2<T> operator*(const Vector2<T>& lhs, const U& rhs) {
    Vector2<T> temp = lhs;
    temp *= rhs;
    return temp;
}

template <typename T>
constexpr Vector2<T> operator*(const T& lhs, const Vector2<T>& rhs) {
    Vector2<T> temp = rhs;
    temp *= lhs;
    return temp;
}

template <typename T>
constexpr Vector2<T> operator/(const Vector2<T>& lhs, const T& rhs) {
    Vector2<T> temp = lhs;
    temp /= rhs;
    return temp;
}

template <typename T>
constexpr Vector2<T> operator%(const Vector2<T>& lhs, const T& rhs) {
    Vector2<T> temp = lhs;
    temp %= rhs;
    return temp;
}

} // namespace geom
