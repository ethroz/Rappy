#pragma once

#include <map>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <boost/json.hpp>

#include "wiring/Pin.hpp"
#include "wiring/PinConfig.hpp"

#include "Color.hpp"

namespace light {

enum class LightName : uint8_t {
    RGB_LED,
    DUAL_COLOR_LED
};

class Light {
public:
    static Light create(const boost::json::object& cfg);

    void color(const Color& col);
    Color color() const { return m_color; }
    LightName name() const { return m_name; }

private:
    Light(LightName name, std::span<const int> pins, wiring::PinConfig config);
    
    const LightName m_name;
    Color m_color;
    std::vector<std::unique_ptr<wiring::Pin>> m_pins;
};


} // namespace light

