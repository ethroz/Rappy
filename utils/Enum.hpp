#pragma once

#include <algorithm>
#include <cstdint>
#include <format>
#include <map>
#include <stdexcept>
#include <string_view>

#include "Other.hpp"

#define CREATE_ENUM(EnumName, ...) \
enum class EnumName : uint8_t { __VA_ARGS__ };

#define CREATE_ENUM_FROM_STRING(EnumName, ...) \
static const std::map<std::string_view, EnumName> EnumName##_MAP = { \
    PAIR_LIST(EnumName, __VA_ARGS__) \
}; \
EnumName EnumName##FromString(const std::string_view mode) { \
    const auto modeStr = toupper(mode); \
    const auto it = EnumName##_MAP.find(modeStr); \
    if (it == EnumName##_MAP.end()) { \
        throw std::invalid_argument(std::format("Unrecognized control mode: {}", modeStr)); \
    } \
    return it->second; \
}

#define CREATE_ENUM_SET(EnumName, ...) \
CREATE_ENUM(EnumName, __VA_ARGS__) \
CREATE_ENUM_FROM_STRING(EnumName, __VA_ARGS__)

#define PARENS ()

#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define FOR_EACH(macro, temp, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, temp, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, temp, a1, ...)                         \
  macro(temp, a1)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, temp, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#define PAIR_LIST(EnumName, ...) FOR_EACH(PAIR, EnumName, __VA_ARGS__)
#define PAIR(EnumName, x) {#x, EnumName::x},

// Example usage:
// CREATE_ENUM(ControlMode, SOLID, CYCLE, FLASH)
// CREATE_ENUM_FROM_STRING(ControlMode, SOLID, CYCLE, FLASH)
