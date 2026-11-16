#pragma once

#include <string>
#include <type_traits>
#include <optional>

#ifdef COMPONENTS_IS_ENABLED_QT
#include <QString>
#include <QStringRef>
#endif // Qt only

namespace Logger
{

template <typename T, typename = void>
struct IsString : public std::false_type { };

template <typename T>
struct IsString<
    T,
    std::enable_if_t<
        std::disjunction_v<
        std::is_same<T, std::string>
        , std::is_same<T, std::string_view>

#ifdef COMPONENTS_IS_ENABLED_QT
        , std::is_same<T, QString>
        , std::is_same<T, QStringRef>
#endif // Qt only

        > >
    > : public std::true_type { };


// ======================================================= //

template <typename T, typename = void>
struct IsContainer : public std::false_type { };

template <typename T>
struct IsContainer<
    T,
    std::void_t<
        decltype(T().begin()),
        decltype(T().end()),
        std::enable_if_t<!IsString<T>::value>
        > > : public std::true_type { };


// ======================================================= //

template <typename T, typename = void>
struct IsMap : public std::false_type { };

template <typename T>
struct IsMap<
    T,
    std::void_t<
        IsContainer<T>,
        typename T::key_type,
        typename T::value_type
        > > : public std::true_type { };


// ======================================================= //


template <class T>
struct IsOptional : std::false_type {};

// Specialization: it is std::optional<U>
template <class U>
struct IsOptional<std::optional<U>> : std::true_type {};

}
