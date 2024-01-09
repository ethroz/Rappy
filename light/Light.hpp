#pragma once

#include <map>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wiring/Pin.hpp"
#include "wiring/PinConfig.hpp"
#include "Color.hpp"

namespace light {

enum class LightName : uint8_t {
    RGB_LED,
    DUAL_COLOR_LED
};

enum class LightMode : uint8_t {
    DIGITAL,
    PWM
};

class Light {
public:
    static Light create(LightName name, LightMode mode, std::span<int> pins);
    static LightName nameFromString(std::string_view name);
    static LightMode modeFromString(std::string_view mode);

    void color(const Color& col);
    Color color() const { return m_color; }
    LightName name() const { return m_name; }

private:
    Light(LightName name, std::span<int> pins, wiring::PinConfig config);
    
    const LightName m_name;
    Color m_color;
    std::vector<std::unique_ptr<wiring::Pin>> m_pins;
};

} // namespace light

