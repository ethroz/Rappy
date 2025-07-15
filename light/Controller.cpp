#include <cmath>
#include <format>
#include <stdexcept>

#include "utils/Duration.hpp"
#include "utils/Enum.hpp"
#include "utils/JsonHelper.hpp"
#include "utils/Logger.hpp"
#include "utils/Other.hpp"

#include "Controller.hpp"

namespace light {

CREATE_ENUM_SET(ControlMode, SOLID, CYCLE, FLASH)

std::unique_ptr<Controller> Controller::create(const boost::json::object& cfg, Light&& light) {
    const auto mode = ControlModeFromString(getAsOrThrow<std::string_view>(cfg, "mode", "light::Controller::create()"));
    const auto color = Color::fromString(getAsOr<std::string_view>(cfg, "color", "white"));
    const auto brightness = getAsOr<float>(cfg, "brightness", 1.0f);
    const auto period = getAsDurationOr(cfg, "period", 1.0f).get();

    if (mode != ControlMode::CYCLE && light.name() == LightName::DUAL_COLOR_LED && color.B > 0.0f) {
        logger.warning() << "That color will not be represented properly on the Dual-Color LED";
    }

    switch (mode) {
    case ControlMode::SOLID: return std::make_unique<SolidLight>(std::move(light), color, brightness);
    case ControlMode::CYCLE: return std::make_unique<LightCycle>(std::move(light), brightness, period);
    case ControlMode::FLASH: return std::make_unique<FlashingLight>(std::move(light), color, brightness, period);
    default: throw std::invalid_argument(std::format("light::Controller::create(): Invalid control mode: {}", to_underlying(mode)));
    }
}

CREATE_ENUM_SET(LightControl, COLOR, BRIGHTNESS, PERIOD)

pi::Consumer SolidLight::getConsumer(std::string_view key) {
    const auto control = LightControlFromString(key.substr(0, key.find('.')));
    switch (control) {
    case LightControl::COLOR: {
        if (key.find('.') == std::string_view::npos) {
            return [this](float value) { m_color = value; };
        }
        else {
            const auto channel = key.substr(key.find('.') + 1);
            if (channel.size() != 1) {
                throw std::invalid_argument(std::format("Unrecognized color channel: {}", channel));
            }
            switch (std::tolower(channel[0])) {
            case 'r': return [this](float value) { m_color.R = value; };
            case 'g': return [this](float value) { m_color.G = value; };
            case 'b': return [this](float value) { m_color.B = value; };
            default: throw std::invalid_argument(std::format("Unrecognized color channel: {}", channel));
            }
        }
    }
    case LightControl::BRIGHTNESS: return [this](float value) { m_brightness = value; };
    default: throw std::invalid_argument(std::format("Unrecognized light control: {}", key));
    }
}

pi::Consumer LightCycle::getConsumer(std::string_view key) {
    const auto control = LightControlFromString(key);
    switch (control) {
    case LightControl::BRIGHTNESS: return [this](float value) { m_brightness = value; };
    case LightControl::PERIOD: return [this](float value) { m_period = value; };
    default: throw std::invalid_argument(std::format("Unrecognized light control: {}", key));
    }
}

pi::Consumer FlashingLight::getConsumer(std::string_view key) {
    const auto control = LightControlFromString(key.substr(0, key.find('.')));
    switch (control) {
    case LightControl::COLOR: {
        if (key.find('.') == std::string_view::npos) {
            return [this](float value) { m_color = value; };
        }
        else {
            const auto channel = key.substr(key.find('.') + 1);
            if (channel.size() != 1) {
                throw std::invalid_argument(std::format("Unrecognized color channel: {}", channel));
            }
            switch (std::tolower(channel[0])) {
            case 'r': return [this](float value) { m_color.R = value; };
            case 'g': return [this](float value) { m_color.G = value; };
            case 'b': return [this](float value) { m_color.B = value; };
            default: throw std::invalid_argument(std::format("Unrecognized color channel: {}", channel));
            }
        }
    }
    case LightControl::BRIGHTNESS: return [this](float value) { m_brightness = value; };
    case LightControl::PERIOD: return [this](float value) { m_period = value; };
    default: throw std::invalid_argument(std::format("Unrecognized light control: {}", key));
    }
}

void SolidLight::step() {
    const Color col = m_color * m_brightness;
    logger.debug() << "light::SolidLight::step(): Setting color: " << col;
    m_light.color(col);
}
   
void LightCycle::step() {
    float hue = m_timer.elapsed() * 360.0f / m_period;
    if (hue > 360.0f) {
        m_timer.reset();
        hue = 360.0f;
    }
    const Color col = Color::fromHsv(hue, 1.0f, m_brightness);
    logger.debug() << "light::LightCycle::step(): Setting color: " << col;
    m_light.color(col);
}

void FlashingLight::step() {
    const Color col = m_color * m_brightness * ((sin(2.0f * float(M_PI) * m_timer.elapsed() / m_period) + 1.0f) / 2.0f);
    logger.debug() << "light::FlashingLight::step(): Setting color: " << col;
    m_light.color(col);
}

} // namespace light

