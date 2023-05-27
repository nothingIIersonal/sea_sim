#pragma once

#include "vector.hpp"

#include <initializer_list>
#include <vector>

namespace geom {

template <typename VectorType>
class Polygon {
public:
    // anti-clockwise order iterators
    using ACLockIt = std::vector<VectorType>::iterator;
    using ConstAClockIt = std::vector<VectorType>::const_iterator;
    // clockwise order iterators
    using CLockIt = std::vector<VectorType>::reverse_iterator;
    using ConstClockIt = std::vector<VectorType>::const_reverse_iterator;
public:
    using value_type = VectorType;
    using const_value_type = const VectorType;
    using value_type_ptr = VectorType*;
    using const_value_type_ptr = const VectorType*;
public:
    Polygon() = default;
    explicit Polygon(std::size_t capacity = 0);
    Polygon(std::initializer_list<VectorType> points);

    void Reserve(std::size_t capacity);
    template <typename... Args>
    void Emplace(Args&&... args);
    void Push(const VectorType& point);
    void Push(VectorType&& point);

    [[nodiscard]] bool Empty() const;
    [[nodiscard]] std::size_t Size() const;
    const VectorType& operator[](std::size_t pos) const;
    VectorType& operator[](std::size_t pos);

    auto begin();
    auto end();
    auto cbegin() const;
    auto cend() const;

    auto rbegin();
    auto rend();
    auto crbegin() const;
    auto crend() const;
private:
    std::vector<VectorType> points_;
};

template <typename VectorType>
Polygon<VectorType>::Polygon(std::size_t capacity) {
    points_.reserve(capacity);
}

template <typename VectorType>
Polygon<VectorType>::Polygon(std::initializer_list<VectorType> points)
    : points_(points) {
}

template <typename VectorType>
void Polygon<VectorType>::Reserve(std::size_t capacity) {
    points_.reserve(capacity);
}

template <typename VectorType>
template <typename... Args>
void Polygon<VectorType>::Emplace(Args&&... args) {
    points_.emplace_back(std::forward<Args>(args)...);
}

template <typename VectorType>
void Polygon<VectorType>::Push(const VectorType& point) {
    points_.push_back(point);
}

template <typename VectorType>
void Polygon<VectorType>::Push(VectorType&& point) {
    points_.push_back(std::move(point));
}

template <typename VectorType>
bool Polygon<VectorType>::Empty() const {
    return points_.empty();
}

template <typename VectorType>
std::size_t Polygon<VectorType>::Size() const {
    return points_.size();
}

template <typename VectorType>
auto Polygon<VectorType>::begin() {
    return points_.begin();
}

template <typename VectorType>
auto Polygon<VectorType>::end() {
    return points_.end();
}

template <typename VectorType>
auto Polygon<VectorType>::cbegin() const {
    return points_.cbegin();
}

template <typename VectorType>
auto Polygon<VectorType>::cend() const {
    return points_.cend();
}

template <typename VectorType>
auto Polygon<VectorType>::rbegin() {
    return points_.rbegin();
}

template <typename VectorType>
auto Polygon<VectorType>::rend() {
    return points_.rend();
}

template <typename VectorType>
auto Polygon<VectorType>::crbegin() const {
    return points_.crbegin();
}

template <typename VectorType>
auto Polygon<VectorType>::crend() const {
    return points_.crend();
}

template <typename VectorType>
const VectorType& Polygon<VectorType>::operator[](std::size_t pos) const {
    return points_[pos];
}

template <typename VectorType>
VectorType& Polygon<VectorType>::operator[](std::size_t pos) {
    return points_[pos];
}

} // namespace geom
