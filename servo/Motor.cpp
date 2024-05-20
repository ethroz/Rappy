#include <cmath>
#include <map>
#include <stdexcept>
#include <fmt/format.h>

#include "utils/Other.hpp"

#include "Motor.hpp"

namespace servo {

static const std::map<std::string_view, MotorName> NAME_MAP = {
    {"fs90r", MotorName::FS90R},
    {"l298n", MotorName::L298N}
};

MotorName Motor::nameFromString(std::string_view name) {
    const auto nameStr = tolower(name);

    const auto it = NAME_MAP.find(nameStr);
    if (it == NAME_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized Motor name: {}", name));
    }

    return it->second;
}

Motor::Motor(MotorName name, const wiring::PinConfig& config) :
    m_pin(wiring::Pin::create(config)),
    m_name(name)
{}

std::unique_ptr<Motor> Motor::create(MotorName name, std::vector<int> pins) {
    std::vector<wiring::PinConfig> configs(pins.size());
    for (size_t i = 0; i < pins.size(); i++) {
        configs[i].pin = pins[i];
    }

    switch (name) {
    case MotorName::FS90R:
        if (pins.size() != 1) {
            throw std::invalid_argument("Expected 1 pin for the FS90R servo");
        }
        configs[0].mode = wiring::PinMode::SERVO;
        return std::make_unique<Fs90r>(configs[0]);
    case MotorName::L298N:
        if (pins.size() != 2) {
            throw std::invalid_argument("Expected 2 pins for the L298N servo");
        }
        for (auto& config : configs) {
            config.mode = wiring::PinMode::PWM;
        }
        return std::make_unique<L298n>(configs[0], configs[1]);
    default: throw std::logic_error(fmt::format("Unexpected Motor enum value: {}", to_underlying(name)));
    }
}

void Fs90r::set(float value) {
    value = std::clamp(value, -1.0f, 1.0f);
    m_value = value;
    value = powf(fabs(value), 2.17391304348f) * sign(value);
    value *= 0.3f;
    m_pin->set(value);
}

L298n::L298n(const wiring::PinConfig& fwd, const wiring::PinConfig& bwd) :
    Motor(MotorName::L298N, fwd),
    m_oPin(wiring::Pin::create(bwd))
{}

void L298n::set(float value) {
    value = std::clamp(value, -1.0f, 1.0f);
    m_value = value;
    m_pin->set(value);
    m_oPin->set(-value);
}

} // namespace light

