#include <algorithm>
#include <cmath>
#include <format>
#include <map>
#include <stdexcept>

#include "utils/Other.hpp"

#include "Color.hpp"

namespace light {

static const std::map<std::string_view, Color> COLOR_MAP = {
    {"red",     Color::RED()    },
    {"yellow",  Color::YELLOW() },
    {"green",   Color::GREEN()  },
    {"cyan",    Color::CYAN()   },
    {"blue",    Color::BLUE()   },
    {"magenta", Color::MAGENTA()},
    {"black",   Color::BLACK()  },
    {"white",   Color::WHITE()  }
};

Color Color::fromHsv(float hue, float saturation, float value) {
    hue = std::fmod(hue, 360.0f);
    saturation = std::clamp(saturation, 0.0f, 1.0f);
    value = std::clamp(value, 0.0f, 1.0f);

    const float chroma = value * saturation;
    const float temp = chroma * (1.0f - std::abs(std::fmod(hue / 60.0f, 2.0f) - 1.0f));
    const float offset = value - chroma;

    Color col;
    const auto index = int(hue / 60.0f);
    switch (index) {
    case 0: col = {chroma, temp, 0.0f}; break;
    case 1: col = {temp, chroma, 0.0f}; break;
    case 2: col = {0.0f, chroma, temp}; break;
    case 3: col = {0.0f, temp, chroma}; break;
    case 4: col = {temp, 0.0f, chroma}; break;
    case 5: col = {chroma, 0.0f, temp}; break;
    default: throw std::logic_error(std::format("Expected hue to be in the range [0, 360): {}", hue));
    }
    return col + offset;
}

Color Color::fromString(std::string_view col) {
    const auto colStr = tolower(col);
    if (!COLOR_MAP.contains(colStr)) {
        throw std::invalid_argument("color does not exist: " + colStr);
    }

    return COLOR_MAP.at(colStr);
}

float Color::operator[](size_t index) { 
    switch(index) { 
    case 0: return R;
    case 1: return G;
    case 2: return B;
    default: throw std::runtime_error("Out of bounds");
    }
}

std::ostream& operator<<(std::ostream& out, const Color& col) {
    return out << '{' << col.R << ", " << col.G << ", " << col.B << '}';
}

} // namespace light

