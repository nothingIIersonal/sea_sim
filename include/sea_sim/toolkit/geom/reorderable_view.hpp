#pragma once

#include <vector>
#include <algorithm>

namespace geom {

template<typename T>
class ReorderableView {
public:
    ReorderableView() = default;

    template<typename InputIt>
    ReorderableView(InputIt first, InputIt last);

    T*& operator[](std::size_t index);
    const T*& operator[](std::size_t index) const;

    void Reserve(std::size_t capacity);
    void PushBack(const T& value);
    void PushBack(T* value);

    [[nodiscard]] std::size_t Size() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;


    auto begin() noexcept;
    auto end() noexcept;
    auto rbegin() noexcept;
    auto rend() noexcept;
    auto cbegin() const noexcept;
    auto cend() const noexcept;
    auto crbegin() const noexcept;
    auto crend() const noexcept;

private:
    std::vector<T*> values_;
};

template<typename RViewIt, typename Compare>
void Reorder(RViewIt first, RViewIt last, Compare cmp);

//////////////////////////////////////////////////////////////////////////////////////
template<typename RViewIt, typename Compare>
void Reorder(RViewIt first, RViewIt last, Compare cmp) {
    using underlying_type = typename RViewIt::value_type;
    auto wrapper = [&cmp](const underlying_type lhs, const underlying_type rhs) {
        return cmp(*lhs, *rhs);
    };
    std::sort(first, last, wrapper);
}


template<typename T>
template<typename InputIt>
ReorderableView<T>::ReorderableView(InputIt first, InputIt last) {
    values_.reserve(std::distance(first, last));
    auto& res = values_;
    std::for_each(first, last,
                  [&res](auto&& val){
                    res.push_back(&val);
                  });
}

template<typename T>
T*& ReorderableView<T>::operator[](std::size_t index) {
    return values_[index];
}

template<typename T>
const T*& ReorderableView<T>::operator[](std::size_t index) const {
    return values_[index];
}

template<typename T>
auto ReorderableView<T>::begin() noexcept {
    return values_.begin();
}

template<typename T>
auto ReorderableView<T>::end() noexcept {
    return values_.end();
}

template <typename T>
auto ReorderableView<T>::rbegin() noexcept {
    return values_.rbegin();
}

template <typename T>
auto ReorderableView<T>::rend() noexcept {
    return values_.rend();
}

template <typename T>
auto ReorderableView<T>::cbegin() const noexcept {
    return values_.crbegin();
}

template <typename T>
auto ReorderableView<T>::cend() const noexcept {
    return values_.cend();
}

template <typename T>
auto ReorderableView<T>::crend() const noexcept {
    return values_.crend();
}

template <typename T>
auto ReorderableView<T>::crbegin() const noexcept {
    return values_.crbegin();
}

template <typename T>
void ReorderableView<T>::PushBack(const T& value) {
    values_.push_back(&value);
}

template <typename T>
void ReorderableView<T>::PushBack(T* value) {
    values_.push_back(value);
}

template <typename T>
void ReorderableView<T>::Reserve(std::size_t capacity) {
    values_.reserve(capacity);
}

template <typename T>
bool ReorderableView<T>::Empty() const noexcept {
    return values_.empty();
}

template <typename T>
std::size_t ReorderableView<T>::Size() const noexcept {
    values_.size();
}

} // namespace geom
