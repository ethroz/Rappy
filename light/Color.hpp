#pragma once

#include <ostream>
#include <string>

namespace light {

class Color {
public:
    constexpr Color() : R{}, G{}, B{} {}
    constexpr Color(float r) : R(r), G(r), B(r) {}
    constexpr Color(float r, float g) : R(r), G(g), B{} {}
    constexpr Color(float r, float g, float b) : R(r), G(g), B(b) {}

    static Color invert(Color c) { return {1.0f - c.R, 1.0f - c.G, 1.0f - c.B}; }
    static Color fromHsv(float hue, float saturation, float value);
    static Color fromString(std::string_view col);

    static constexpr Color RED()     { return {1.0f, 0.0f, 0.0f}; }
    static constexpr Color YELLOW()  { return {1.0f, 1.0f, 0.0f}; }
    static constexpr Color GREEN()   { return {0.0f, 1.0f, 0.0f}; }
    static constexpr Color CYAN()    { return {0.0f, 1.0f, 1.0f}; }
    static constexpr Color BLUE()    { return {0.0f, 0.0f, 1.0f}; }
    static constexpr Color MAGENTA() { return {1.0f, 0.0f, 1.0f}; }
    static constexpr Color BLACK()   { return {0.0f, 0.0f, 0.0f}; }
    static constexpr Color WHITE()   { return {1.0f, 1.0f, 1.0f}; }



    float operator[](size_t index); 
    Color operator*(float val) const { return {R * val, G * val, B * val}; }
    Color operator+(float val) const { return {R + val, G + val, B + val}; }

    float R;
    float G;
    float B;
};

std::ostream& operator<<(std::ostream& out, const Color& col);

} // namespace light

