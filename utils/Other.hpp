#pragma once

#include <algorithm>
#include <bit>
#include <charconv>
#include <concepts>
#include <format>
#include <limits>
#include <optional>
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

inline std::string plural(size_t n) {
    return n == 1 ? "" : "s";
}

template <std::integral T>
T to(std::string_view input) {
    T out;
    const auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), out, 10);
    if(ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
        throw std::invalid_argument(std::format("String is not an integer: {}", input));
    }
    return out;
}

template <std::floating_point T>
T to(std::string_view input) {
    static_assert(std::is_same_v<T, double> || std::is_same_v<T, float>);
    if constexpr (std::is_same_v<T, double>) {
        return std::stod(std::string(input));
    }
    else {
        return std::stof(std::string(input));
    }
    // T out;
    // const auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), out, std::chars_format::general);
    // if(ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
    //     return std::nullopt;
    // }
    // return out;
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

