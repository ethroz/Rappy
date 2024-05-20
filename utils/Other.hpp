#pragma once

#include <algorithm>
#include <bit>
#include <concepts>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

inline std::string tolower(std::string_view sv) {
    std::string s(sv);
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

inline std::string toupper(std::string_view sv) {
    std::string s(sv);
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
    return s;
}

template <typename T>
constexpr T transform(T value, T min, T max) {
    return value * (max - min) + min;
}

template <typename T>
constexpr T map(T value, T oldMin, T oldMax, T newMin, T newMax) {
    return transform((value - oldMin) / (oldMax - oldMin), newMin, newMax);
}

template <typename T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
constexpr auto to_underlying(T t) { return static_cast<std::underlying_type_t<T>>(t); }

template <std::integral T>
constexpr T byteswap(T value) noexcept {
    auto byteView = std::span(reinterpret_cast<uint8_t*>(&value), sizeof(T));
    std::ranges::reverse(byteView.begin(), byteView.end());
    return value;
}

template <typename T>
constexpr int sign(T val) {
    return int(T(0) < val) - int(val < T(0));
}

template <std::integral T>
constexpr float normalize(T val) {
    return float(val) / std::numeric_limits<decltype(val)>::max();
}

