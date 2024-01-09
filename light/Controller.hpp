#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include "utils/Logger.hpp"
#include "utils/Timer.hpp"
#include "Color.hpp"
#include "Light.hpp"

namespace light {

enum class ControlMode : uint8_t {
    SOLID,
    CYCLE,
    FLASH
};

class Controller {
public:
    virtual ~Controller() {}

    static std::unique_ptr<Controller> create(
        ControlMode mode, 
        Light&& light, 
        const Color& color,
        float brightness,
        float period,
        const Logger& logger);
    static ControlMode modeFromString(std::string_view modeStr);

    virtual void step() = 0;
    virtual Color color() { return m_light.color(); }

protected:
    Controller(Light&& light) : m_light(std::move(light)) {}

    Light m_light;
};

class SolidLight : public Controller {
public:
    SolidLight(Light&& light, const Color& color) : Controller(std::move(light)) { m_light.color(color); }

    void step() override {}
};

class LightCycle : public Controller {
public:
    LightCycle(Light&& light, float brightness, float period) : 
        Controller(std::move(light)),
        m_brightness(brightness),
        m_period(period)
    { m_timer.start(); }
    
    void step() override;

private:
    Timer m_timer;
    const float m_brightness;
    const float m_period;
};

class FlashingLight : public Controller {
public:
    FlashingLight(Light&& light, const Color& color, float period) : 
        Controller(std::move(light)), 
        m_color(color),
        m_period(period)
    { m_timer.start(); }

    void step() override;

private:
    Timer m_timer;
    const Color m_color;
    const float m_period;
};

} // namespace light

