#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include <boost/json.hpp>

#include "pi/Output.hpp"
#include "utils/Timer.hpp"

#include "Color.hpp"
#include "Light.hpp"

namespace light {

class Controller : public pi::Output {
public:
    virtual ~Controller() {}

    static std::unique_ptr<Controller> create(const boost::json::object& cfg, Light&& light);

    virtual Color color() { return m_light.color(); }

protected:
    Controller(Light&& light, std::string_view type) :
        pi::Output(type),
        m_light(std::move(light))
    {}

    Light m_light;
};

class SolidLight : public Controller {
public:
    SolidLight(Light&& light, const Color& color, float brightness) :
        Controller(std::move(light), "SolidLight"),
        m_color(color),
        m_brightness(brightness)
    {}
    
    pi::Consumer getConsumer(std::string_view key) override;

    void step() override;

private:
    Color m_color;
    float m_brightness;
};

class LightCycle : public Controller {
public:
    LightCycle(Light&& light, float brightness, float period) : 
        Controller(std::move(light), "LightCycle"),
        m_timer(true),
        m_brightness(brightness),
        m_period(period)
    {}
    
    pi::Consumer getConsumer(std::string_view key) override;

    void step() override;

private:
    Timer m_timer;
    float m_brightness;
    float m_period;
};

class FlashingLight : public Controller {
public:
    FlashingLight(Light&& light, const Color& color, float brightness, float period) : 
        Controller(std::move(light), "FlashingLight"), 
        m_timer(true),
        m_color(color),
        m_brightness(brightness),
        m_period(period)
    {}

    pi::Consumer getConsumer(std::string_view key) override;

    void step() override;

private:
    Timer m_timer;
    Color m_color;
    float m_brightness;
    float m_period;
};

} // namespace light

