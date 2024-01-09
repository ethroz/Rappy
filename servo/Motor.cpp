#include <cmath>
#include <map>
#include <stdexcept>
#include <fmt/core.h>

#include "utils/Other.hpp"

#include "Motor.hpp"

namespace servo {

static const std::map<std::string_view, MotorName> NAME_MAP = {
    {"fs90r", MotorName::FS90R}
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

std::unique_ptr<Motor> Motor::create(MotorName name, int pin) {
    wiring::PinConfig config{};
    config.pin = pin;
    config.mode = wiring::PinMode::SERVO;

    switch (name) {
    case MotorName::FS90R: return std::make_unique<Fs90r>(config);
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

} // namespace light

