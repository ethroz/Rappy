#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>
#include <fmt/core.h>

#include "utils/Other.hpp"

#include "Controller.hpp"

namespace servo {

static const std::map<std::string_view, ControlMode> MODE_MAP = {
    {"constant", ControlMode::CONSTANT},
    {"oscillate",   ControlMode::OSCILLATE},
    {"dance",       ControlMode::DANCE}
};

ControlMode Controller::modeFromString(std::string_view mode) {
    const auto modeStr = tolower(mode);

    const auto it = MODE_MAP.find(modeStr);
    if (it == MODE_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized control mode: {}", modeStr));
    }

    return it->second;
}

std::unique_ptr<Controller> Controller::create(
    ControlMode mode, 
    std::unique_ptr<Motor>&& motor, 
    float max,
    float period)
{
    period = std::max(period, 0.0f);

    switch (mode) {
    case ControlMode::CONSTANT:
        max = std::clamp(max, -1.0f, 1.0f);
        return std::make_unique<ConstantMotor>(std::move(motor), max);
    case ControlMode::OSCILLATE:
        max = std::clamp(max, 0.0f, 1.0f);
        return std::make_unique<OscillatorMotor>(std::move(motor), max, period);
    case ControlMode::DANCE:
        return std::make_unique<DancingMotor>(std::move(motor), period);
    default: throw std::invalid_argument(fmt::format("Invalid control mode: {}", to_underlying(mode)));
    }
}

void OscillatorMotor::step() {
    const auto elapsed = m_timer.elapsed();
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

    const auto elapsed = m_timer.elapsed();
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

} // namespace servo

