#pragma once

#include <memory>
#include <string_view>

#include "wiring/Pin.hpp"
#include "wiring/PinConfig.hpp"

namespace servo {

enum class MotorName : uint8_t {
    FS90R
};

class Motor {
public:
    static std::unique_ptr<Motor> create(MotorName name, int pin);
    static MotorName nameFromString(std::string_view name);

    // Sign follows right hand rule when looking at the top of the motor.
    virtual void set(float value) = 0;
    virtual float get() const { return m_value; }
    MotorName name() const { return m_name; }

protected:
    Motor(MotorName name, const wiring::PinConfig& config);
    
    const std::unique_ptr<wiring::Pin> m_pin;
    const MotorName m_name;
    float m_value;
};

class Fs90r : public Motor {
public:
    Fs90r(const wiring::PinConfig& config) : Motor(MotorName::FS90R, config) {}

    void set(float value) override;
};

} // namespace servo

