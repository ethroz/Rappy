#include <cmath>
#include <stdexcept>
#include <fmt/core.h>

#include "utils/Logger.hpp"
#include "utils/Other.hpp"

#include "Controller.hpp"

namespace light {

static const std::map<std::string_view, ControlMode> MODE_MAP = {
    {"solid", ControlMode::SOLID},
    {"cycle", ControlMode::CYCLE},
    {"flash", ControlMode::FLASH}
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
    Light&& light, 
    const Color& color,
    float brightness,
    float period)
{
    if (mode != ControlMode::CYCLE && light.name() == LightName::DUAL_COLOR_LED && color.B > 0.0f) {
        logger.warning() << "That color will not be represented properly on the Dual-Color LED";
    }

    switch (mode) {
    case ControlMode::CYCLE: return std::make_unique<LightCycle>(std::move(light), brightness, period);
    case ControlMode::SOLID: return std::make_unique<SolidLight>(std::move(light), color * brightness);
    case ControlMode::FLASH: return std::make_unique<FlashingLight>(std::move(light), color * brightness, period);
    default: throw std::invalid_argument(fmt::format("Invalid control mode: {}", to_underlying(mode)));
    }
}
   
void LightCycle::step() {
    float hue = m_timer.elapsed() * 360.0f / m_period;
    if (hue > 360.0f) {
        m_timer.reset();
        hue = 360.0f;
    }
    const Color col = Color::fromHsv(hue, 1.0f, m_brightness);
    m_light.color(col);
}

void FlashingLight::step() {
    const Color col = m_color * ((sin(2.0f * float(M_PI) * m_timer.elapsed() / m_period) + 1.0f) / 2.0f);
    m_light.color(col);
}

} // namespace light

