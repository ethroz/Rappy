#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include <boost/json.hpp>

#include "utils/Timer.hpp"
#include "Motor.hpp"

namespace motor {

class Controller : public pi::Output {
public:
    virtual ~Controller() {}

    static std::unique_ptr<Controller> create(const boost::json::object& cfg, std::unique_ptr<Motor>&& motor);

    virtual float value() const { return m_motor->get(); }

protected:
    Controller(std::unique_ptr<Motor>&& motor, std::string_view type) :
        pi::Output(type),
        m_motor(std::move(motor))
    {}

    const std::unique_ptr<Motor> m_motor;
};

class ConstantMotor : public Controller {
public:
    ConstantMotor(std::unique_ptr<Motor>&& motor, float dir) :
        Controller(std::move(motor), "ConstantMotor")
    {}

    pi::Consumer getConsumer(std::string_view key) override;

    void step() override;

private:
    float m_value;
};

class OscillatorMotor : public Controller {
public:
    OscillatorMotor(std::unique_ptr<Motor>&& motor, float max, float period) : 
        Controller(std::move(motor), "OscillatorMotor"),
        m_timer(true),
        m_max(max),
        m_period(period)
    {}
    
    pi::Consumer getConsumer(std::string_view key) override;

    void step() override;

private:
    Timer m_timer;
    float m_max;
    float m_period;
};

class DancingMotor : public Controller {
public:
    DancingMotor(std::unique_ptr<Motor>&& motor, float startPeriod) : 
        Controller(std::move(motor), "DancingMotor"),
        m_startPeriod(startPeriod)
    {}
    
    pi::Consumer getConsumer(std::string_view key) override;

    void step() override;

private:
    Timer m_timer;
    const float m_startPeriod;
    float m_period;
};

} // namespace motor

