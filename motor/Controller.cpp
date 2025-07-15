#include <algorithm>
#include <cmath>
#include <format>
#include <map>
#include <stdexcept>

#include "utils/Enum.hpp"
#include "utils/JsonHelper.hpp"
#include "utils/Other.hpp"

#include "Controller.hpp"

namespace motor {

CREATE_ENUM_SET(ControlMode, CONSTANT, OSCILLATE, DANCE)

std::unique_ptr<Controller> Controller::create(const boost::json::object& cfg, std::unique_ptr<Motor>&& motor) {
    const auto modeStr = getAsOr<std::string_view>(cfg, "mode", "constant");
    const auto mode = ControlModeFromString(modeStr);
    const auto period = getAsDurationOr(cfg, "period", 1.0f).get();
    if (period < 0.0f) {
        throw std::invalid_argument("Cannot have a negative motor period");
    }

    switch (mode) {
    case ControlMode::CONSTANT: {
        const auto value = getAsOr<float>(cfg, "value", 1.0f);
        if (value < 0.0f || value > 1.0f) {
            throw std::invalid_argument("A motor \"value\" cannot be outside of [-1.0, 1.0]");
        }
        return std::make_unique<ConstantMotor>(std::move(motor), value);
    }
    case ControlMode::OSCILLATE: {
        const auto max = getAsOr<float>(cfg, "max", 1.0f);
        if (max < 0.0f || max > 1.0f) {
            throw std::invalid_argument("A motor \"max\" cannot be outside of [0.0, 1.0]");
        }
        return std::make_unique<OscillatorMotor>(std::move(motor), max, period);
    }
    case ControlMode::DANCE:
        return std::make_unique<DancingMotor>(std::move(motor), period);
    default: throw std::invalid_argument(std::format("Invalid control mode: {}", modeStr));
    }
}

CREATE_ENUM_SET(MotorControl, PERIOD, MAX, VALUE)

pi::Consumer ConstantMotor::getConsumer(std::string_view key) {
    const auto control = MotorControlFromString(key);
    switch (control) {
    case MotorControl::VALUE: return [this](float value) { m_value = value; };
    default: throw std::invalid_argument(std::format("Unrecognized motor control: {}", key));
    }
}

pi::Consumer OscillatorMotor::getConsumer(std::string_view key) {
    const auto control = MotorControlFromString(key);
    switch (control) {
    case MotorControl::PERIOD: return [this](float value) { m_period = value; };
    case MotorControl::MAX: return [this](float value) { m_max = value; };
    default: throw std::invalid_argument(std::format("Unrecognized motor control: {}", key));
    }
}

pi::Consumer DancingMotor::getConsumer(std::string_view key) {
    const auto control = MotorControlFromString(key);
    switch (control) {
    case MotorControl::PERIOD: return [this](float value) { m_period = value; };
    default: throw std::invalid_argument(std::format("Unrecognized motor control: {}", key));
    }
}

void ConstantMotor::step() {
    m_motor->set(m_value);
}

void OscillatorMotor::step() {
    const float elapsed = m_timer.elapsed();
    float val = sin(float(M_PI) * 2.0f / m_period * elapsed);
    if (elapsed > m_period) {
        m_timer.reset();
        val = 0.0f;
    }

    m_motor->set(val * m_max);
}

void DancingMotor::step() {
    if (!m_timer.running()) {
        m_timer.start();
        m_period = m_startPeriod;
    }

    const float elapsed = m_timer.elapsed();
    float val = sin(float(M_PI) * 2.0f / m_period * elapsed);
    if (elapsed > m_startPeriod * 2.0f) {
        m_timer.reset();
        val = 0.0f;
        m_period /= 2.0f;
    }

    if (m_period <= m_startPeriod / 16) {
        m_timer.stop();
    }
    
    m_motor->set(val);
}

} // namespace motor

