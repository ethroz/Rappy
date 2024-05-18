#pragma once

#include <cstdint>

template <size_t N>
struct SizedInt {};

template <>
struct SizedInt<1> { using type = int8_t; };
template <>
struct SizedInt<2> { using type = int16_t; };
template <>
struct SizedInt<4> { using type = int32_t; };
template <>
struct SizedInt<8> { using type = int64_t; };

template <size_t N>
struct SizedUint {};

template <>
struct SizedUint<1> { using type = uint8_t; };
template <>
struct SizedUint<2> { using type = uint16_t; };
template <>
struct SizedUint<4> { using type = uint32_t; };
template <>
struct SizedUint<8> { using type = uint64_t; };

