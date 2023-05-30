#pragma once

#include "polygon.hpp"
#include "reorderable_view.hpp"
#include "vector.hpp"

#include <stack>
#include <tuple>

namespace geom {

namespace ___impl {

template <typename T>
bool IntersectsProjections(T a, T b, T c, T d) {
    if (a > b) {
        std::swap(a, b);
    }
    if (c > d) {
        std::swap(c, d);
    }
    return std::max(a, c) <= std::min(b, d);
}

template <typename T>
bool IntersectsOnSameLine(const Vector2<T>& A, const Vector2<T>& B, /*first  segment*/
                          const Vector2<T>& C, const Vector2<T>& D) /*second segment*/ {
    return IntersectsProjections(A.x, B.x, C.x, D.x) && IntersectsProjections(A.y, B.y, C.y, D.y);
}

} // namespace ___impl

template <typename T>
long double Area(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

template <typename T>
bool Intersects(const Vector2<T>& a0, const Vector2<T>& b0, /*first segment*/
                const Vector2<T>& a1, const Vector2<T>& b1 /*second segment*/) {
    return IntersectsOnSameLine(a0, b0, a1, b1) && (Area(a0, b0, a1) * Area(a0, b0, b1) <= 0) &&
           (Area(a1, b1, a0) * Area(a1, b1, b0) <= 0);
}

template <typename T>
bool Intersects(vec2_ptr<T> a0, vec2_ptr<T> b0, /*first segment*/
                vec2_ptr<T> a1, vec2_ptr<T> b1 /*second segment*/) {
    return Intersects(*a0, *b0, *a1, *b1);
}

template <typename T>
bool Intersects(const Vector2<T>& a, const Vector2<T>& b, /*segment*/
                const Vector2<T>& point) {
    return false;
}

template <typename T>
ReorderableView<Vector2<T>> Triangulate(const Polygon<Vector2<T>>& src) {
    ReorderableView<Vector2<T>> points(src.begin(), src.end());

    // clang-format off
    Reorder(points.begin(), points.end(),
            [](const Vector2<T>& lhs, const Vector2<T>& rhs){
                return lhs.y > rhs.y;
            });
    // clang-format on

    ReorderableView<Vector2<T>> result;
    result.Reserve(src.Size());

    std::stack<vec2_ptr<T>> s;

    s.push(points[0]);
    s.push(points[1]);

    for (std::size_t i = 2; i < points.Size(); ++i) {
        if (points[i] == s.top()) {

            while (!s.empty()) {
                if (s.size() != 1) {
                    result.PushBack(s.top());
                }
                s.pop();
            }
            s.push(points[i - 1]);
            s.push(points[i]);
        } else {
            vec2_ptr<T> last = s.top();
            s.pop();
            while (!Intersects(points[i], s.top())) {
                last = s.top();
                s.pop();
                /// Добавление ребра
            }
            s.push(last);
            s.push(points[i]);
        }
    }

    s.pop();

    while (!s.empty()) {
        /// Странная логика
        result.PushBack(s.top());
        s.pop();
    }

    return result;
}

} // namespace geom
