#include <stdexcept>
#include <fmt/core.h>

#include "utils/Other.hpp"

#include "Light.hpp"

namespace light {

static const std::map<std::string_view, LightName> NAME_MAP = {
    {"rgb led",        LightName::RGB_LED       },
    {"dual-color led", LightName::DUAL_COLOR_LED}
};

static const std::map<std::string_view, LightMode> MODE_MAP = {
    {"digital", LightMode::DIGITAL},
    {"pwm",     LightMode::PWM    }
};

static const std::map<LightMode, wiring::PinMode> PIN_MODE_MAP = {
    {LightMode::DIGITAL, wiring::PinMode::OUT},
    {LightMode::PWM,     wiring::PinMode::PWM}
};

LightName Light::nameFromString(std::string_view name) {
    const auto nameStr = tolower(name);

    const auto it = NAME_MAP.find(nameStr);
    if (it == NAME_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized LED name: {}", name));
    }

    return it->second;
}

LightMode Light::modeFromString(std::string_view mode) {
    const auto modeStr = tolower(mode);

    const auto it = MODE_MAP.find(modeStr);
    if (it == MODE_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized light mode: {}", modeStr));
    }

    return it->second;
}

Light::Light(LightName name, std::span<int> pins, wiring::PinConfig config) : m_name(name) {
    if (pins.size() < 1 || pins.size() > 3) {
        throw std::invalid_argument(fmt::format("Unsupported number of pins: {}", pins.size()));
    }

    m_pins.resize(pins.size());
    for (size_t i = 0; i < pins.size(); i++) {
        config.pin = pins[i];
        m_pins[i] = wiring::Pin::create(config);
    }
}

Light Light::create(LightName name, LightMode mode, std::span<int> pins) {
    wiring::PinConfig config{};
    config.mode = PIN_MODE_MAP.at(mode);
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

void Light::color(const Color& col) {
    m_color = col;
    for (size_t i = 0; i < m_pins.size(); i++) {
        m_pins[i]->set(m_color[i]);
    }
}

} // namespace light

