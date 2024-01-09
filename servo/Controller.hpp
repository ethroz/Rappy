#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include "utils/Logger.hpp"
#include "utils/Timer.hpp"
#include "Motor.hpp"

namespace servo {

enum class ControlMode : uint8_t {
    CONSTANT,
    OSCILLATE,
    DANCE
};

class Controller {
public:
    virtual ~Controller() {}

    static std::unique_ptr<Controller> create(
        ControlMode mode,
        std::unique_ptr<Motor>&& motor,
        float max,
        float period);
    static ControlMode modeFromString(std::string_view modeStr);

    virtual void step() = 0;
    virtual float value() { return m_motor->get(); }

protected:
    Controller(std::unique_ptr<Motor>&& motor) : m_motor(std::move(motor)) {}

    const std::unique_ptr<Motor> m_motor;
};

class ConstantMotor : public Controller {
public:
    ConstantMotor(std::unique_ptr<Motor>&& motor, float dir) : Controller(std::move(motor)) { m_motor->set(dir); }

    void step() override {}
};

class OscillatorMotor : public Controller {
public:
    OscillatorMotor(std::unique_ptr<Motor>&& motor, float max, float period) : 
        Controller(std::move(motor)),
        m_max(max),
        m_period(period)
    { m_timer.start(); }
    
    void step() override;

private:
    Timer m_timer;
    const float m_max;
    const float m_period;
};

class DancingMotor : public Controller {
public:
    DancingMotor(std::unique_ptr<Motor>&& motor, float startPeriod) : 
        Controller(std::move(motor)),
        m_startPeriod(startPeriod)
    {}
    
    void step() override;

private:
    Timer m_timer;
    const float m_startPeriod;
    float m_period;
};

} // namespace servo

