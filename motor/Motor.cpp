#include <cmath>
#include <format>
#include <map>
#include <stdexcept>

#include "utils/Other.hpp"

#include "Motor.hpp"

namespace motor {

static const std::map<std::string_view, MotorName> NAME_MAP = {
    {"fs90r",      MotorName::FS90R},
    {"fitec",      MotorName::FS90R},
    {"continuous", MotorName::FS90R},
    {"ms18",       MotorName::MS18 },
    {"micro",      MotorName::MS18 },
    {"180",        MotorName::MS18 },
    {"l298n",      MotorName::L298N},
    {"fast",       MotorName::L298N}
};

MotorName nameFromString(const std::string_view name) {
    const auto nameStr = tolower(name);

    const auto it = NAME_MAP.find(nameStr);
    if (it == NAME_MAP.end()) {
        throw std::invalid_argument(std::format("Unrecognized Motor name: {}", name));
    }

    return it->second;
}

Motor::Motor(MotorName name, const wiring::PinConfig& config) :
    m_pin(wiring::Pin::create(config)),
    m_name(name)
{}

std::unique_ptr<Motor> Motor::create(const boost::json::object& cfg) {
    const auto nameStr = getAsOrThrow<std::string_view>(cfg, "name", "motor::Motor::create()");
    const auto name = nameFromString(nameStr);

    switch (name) {
    case MotorName::FS90R: {
        const auto pin = getAsOrThrow<int>(cfg, "pin", "light::Light::create()");
        wiring::PinConfig config{};
        config.pin = pin;
        config.mode = wiring::PinMode::SERVO;
        return std::make_unique<Fs90r>(config);
    }
    case MotorName::MS18: {
        const auto pin = getAsOrThrow<int>(cfg, "pin", "light::Light::create()");
        wiring::PinConfig config{};
        config.pin = pin;
        config.mode = wiring::PinMode::SERVO;
        return std::make_unique<Ms18>(config);
    }
    case MotorName::L298N: {
        const auto pins = getAsVecOrThrow<int>(cfg, "pins", "light::Light::create()");
        if (pins.size() != 2) {
            throw std::invalid_argument("Expected 2 pins for the L298N motor");
        }
        std::vector<wiring::PinConfig> configs(pins.size());
        for (size_t i = 0; i < pins.size(); i++) {
            configs[i].pin = pins[i];
            configs[i].mode = wiring::PinMode::PWM;
        }
        return std::make_unique<L298n>(configs[0], configs[1]);
    }
    default: throw std::logic_error(std::format("Unexpected Motor name: {}", nameStr));
    }
}

void Fs90r::set(float value) {
    value = std::clamp(value, -1.0f, 1.0f);
    m_value = value;
    value = powf(fabs(value), 2.17391304348f) * sign(value);
    value *= 0.3f;
    m_pin->set(value);
}

// TODO: Tune this.
void Ms18::set(float value) {
    value = std::clamp(value, -1.0f, 1.0f);
    m_value = value;
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

