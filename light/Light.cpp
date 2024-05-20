#include <stdexcept>
#include <vector>

#include <fmt/format.h>

#include "utils/JsonHelper.hpp"
#include "utils/Other.hpp"

#include "Light.hpp"

namespace light {

static const std::map<std::string_view, LightName> NAME_MAP = {
    {"rgb led",        LightName::RGB_LED       },
    {"rgb",            LightName::RGB_LED       },
    {"dual-color led", LightName::DUAL_COLOR_LED},
    {"dual",           LightName::DUAL_COLOR_LED},
    {"rg",             LightName::DUAL_COLOR_LED}
};

static const std::vector<wiring::PinMode> SUPPORTED_MODES = {
    wiring::PinMode::OUT,
    wiring::PinMode::PWM
};

LightName nameFromString(std::string_view name) {
    const auto nameStr = tolower(name);

    const auto it = NAME_MAP.find(nameStr);
    if (it == NAME_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized LED name: {}", name));
    }

    return it->second;
}

Light::Light(LightName name, std::span<const int> pins, wiring::PinConfig config) : m_name(name) {
    if (pins.size() < 1 || pins.size() > 3) {
        throw std::invalid_argument(fmt::format("Unsupported number of pins: {}", pins.size()));
    }

    m_pins.resize(pins.size());
    for (size_t i = 0; i < pins.size(); i++) {
        config.pin = pins[i];
        m_pins[i] = wiring::Pin::create(config);
    }
}

void Light::color(const Color& col) {
    m_color = col;
    for (size_t i = 0; i < m_pins.size(); i++) {
        m_pins[i]->set(m_color[i]);
    }
}

Light Light::create(const json::object& cfg) {
    const auto name = nameFromString(getAsStringViewOrThrow(cfg, "name", "light::Light::create()"));
    const auto modeStr = getAsStringViewOr(cfg, "pin-mode", "pwm");
    const auto mode = wiring::modeFromString(modeStr);
    const auto pins = getAsVecOrThrow<int>(cfg, "pins", "light::Light::create()");

    if (std::find(SUPPORTED_MODES.begin(), SUPPORTED_MODES.end(), mode) == SUPPORTED_MODES.end()) {
        throw std::invalid_argument(fmt::format("Unsupported pin mode: {}", modeStr));
    }
    
    wiring::PinConfig config{};
    config.mode = mode;
    if (config.mode == wiring::PinMode::PWM) {
        config.pwm.range = 255;
        config.pwm.freq = 1000;
    }

    switch (name) {
    case LightName::RGB_LED:
        if (pins.size() != 3) {
            throw std::invalid_argument("RGB LED requires 3 pins");
        }
        config.invert = true;
        return Light(name, pins, config);
    case LightName::DUAL_COLOR_LED:
        if (pins.size() != 2) {
            throw std::invalid_argument("Dual-Color LED requires 2 pins");
        }
        return Light(name, pins, config);
    default:
        throw std::invalid_argument(fmt::format("Unrecognized LED name: {}", to_underlying(name)));
    }
}

} // namespace light

