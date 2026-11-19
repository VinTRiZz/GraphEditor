#pragma once

#include <type_traits>
#include <utility>

namespace GraphEditor {

namespace detail {

template <typename PointT>
concept CConvertibleToPoint = requires(PointT p) {
    p(double(), double());
};
template <typename PointT>
concept CConvertibleToPointWithSetters = requires(PointT p) {
    p.setX(double()); p.setY(double());
};

template <typename PointT>
concept CContainGetterJavaStyleXY= requires(PointT p) { p.x(); p.y(); };
template <typename PointT>
concept CContainGetterXY= requires(PointT p) { p.getX(); p.getY(); };
}

/**
 * @brief The PointAdapter class   Proxy class to work with data abstracting from Qt
 */
struct PointAdapter
{
    double x;
    double y;

    PointAdapter(double ix = {}, double iy = {}) : x {ix}, y {iy} {}

    // Adapter for proper classes
    template <typename T>
    T convert() const {
        if constexpr (std::is_same_v<T, PointAdapter>) {
            return *this;
        } else if constexpr (detail::CConvertibleToPoint<T>) {
            return T(x, y);
        } else if constexpr (detail::CConvertibleToPointWithSetters<T>) {
            T res;
            res.setX(x);
            res.setY(y);
            return res;
        } else {
            static_assert(false, "Invalid type for Point converting to");
        }
    }

    // Adapter for proper classes
    template <typename T>
    static inline decltype(auto) fromPoint(T&& p) {
        using PointT = std::decay_t<T>;
        if constexpr (std::is_same_v<PointT, PointAdapter>) {
            return std::forward<T>(p);
        } else if constexpr (detail::CContainGetterXY<T>) {
            PointAdapter res;
            res.x = p.getX();
            res.y = p.getY();
            return res;
        } else if constexpr (detail::CContainGetterJavaStyleXY<T>) {
            PointAdapter res;
            res.x = p.x();
            res.y = p.y();
            return res;
        } else {
            static_assert(false, "Invalid type to init Point from");
        }
    }
};

inline bool operator ==(const PointAdapter& lhs,
                       const PointAdapter& rhs) {
    return (lhs.x == rhs.x) && (lhs.x == rhs.x);
}
inline bool operator !=(const PointAdapter& lhs,
                       const PointAdapter& rhs) {
    return (lhs.x != rhs.x) || (lhs.x != rhs.x);
}

}