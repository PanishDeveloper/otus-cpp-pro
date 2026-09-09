#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <type_traits>
#include <cstdint>

template <typename T, typename = void>
struct is_container : std::false_type {};

template <typename T>
struct is_container<T, std::void_t<decltype(std::declval<T>().begin()),
                                   decltype(std::declval<T>().end()), typename T::value_type>>
                                   : std::conditional_t<std::is_same_v<std::decay_t<T>,
                                   std::string>, std:: false_type, std::true_type> {};

template <typename T>
struct is_tuple : std::false_type {};

template <typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};

template <typename Tuple>
struct tuple_types_are_same : std::true_type {};

template <typename T>
struct tuple_types_are_same<std::tuple<T>> : std::true_type {};

template <typename T, typename... Rest>
struct tuple_types_are_same<std::tuple<T, Rest...>> : std::conjunction<std::is_same<T, Rest>...> {};

// Целочисленные типы
template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
void print_ip(std::ostream& os, const T& value)
{
    constexpr size_t bytes = sizeof(T);
    const auto* byte_ptr = reinterpret_cast<const uint8_t*>(&value);

    for (size_t i = bytes; i > 0; --i)
    {
        os << static_cast<int>(byte_ptr[i - 1]);
        if (i > 1) os << '.';
    }
    os << "\n";
}

// String
template <typename T, std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
void print_ip(std::ostream& os, const T& value)
{
    os << value << "\n";
}

// Контейнеры
template <typename T, std::enable_if_t<is_container<T>::value && !is_tuple<T>::value, int> = 0>
void print_ip(std::ostream& os, const T& container)
{
    bool first = true;
    for (const auto& elem : container)
    {
        if (!first) os << '.';
        first = false;
        os << elem;
    }
    os << '\n';
}

// Tuple
template <typename T, std::enable_if_t<is_tuple<T>::value && tuple_types_are_same<T>::value, int> = 0>
void print_ip(std::ostream& os, const T& tup)
{
    std::apply([&os](const auto&... elems)
    {
        bool first = true;
        ((os << (first ? "" : ".") << elems, first = false), ...);
        os << "\n";
    }, tup);
}

template<typename T>
void print_ip(const T& value)
{
    print_ip(std::cout, value);
}