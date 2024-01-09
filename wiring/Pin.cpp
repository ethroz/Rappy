#include <algorithm>
#include <stdexcept>
#include <string>
#include <pigpio.h>
#include <fmt/core.h>

#include "utils/Other.hpp"
#include "utils/PigpioError.hpp"

#include "Pin.hpp"

namespace wiring {

int pinRemap(int pin) {
    switch (pin) {
    case 0:  return 2;
    case 1:  return 3;
    case 2:  return 4;
    case 3:  return 17;
    case 4:  return 27;
    case 5:  return 22;
    case 6:  return 10;
    case 7:  return 9;
    case 8:  return 11;
    case 9:  return 0;
    case 10: return 5;
    case 11: return 6;
    case 12: return 13;
    case 13: return 19;
    case 14: return 26;
    case 15: return 21;
    case 16: return 20;
    case 17: return 16;
    case 18: return 12;
    case 19: return 1;
    case 20: return 7;
    case 21: return 8;
    case 22: return 25;
    case 23: return 24;
    case 24: return 23;
    case 25: return 18;
    case 26: return 15;
    case 27: return 14;
    default: throw std::logic_error(fmt::format("Pin is out of range 0-27: {}", pin));
    }
}

Pin::Pin(int pin) : m_pin(pinRemap(pin)) {}

OutputPin::OutputPin(const PinConfig& config) :
    Pin(config.pin),
    m_invert(config.invert)
{
    gpioSetMode(m_pin, PI_OUTPUT);
    set(0.0f);
}

void OutputPin::set(float val) {
    m_val = std::clamp(val, 0.0f, 1.0f);
    const auto write = (m_val < 0.5f) != m_invert ? PI_LOW : PI_HIGH;
    const auto res = gpioWrite(m_pin, write);
    pigpio::checkError(res);
}


PwmPin::PwmPin(const PinConfig& config) :
    OutputPin(config.pin, config.invert)
{
    gpioSetMode(m_pin, PI_ALT5);
    gpioSetPWMrange(m_pin, config.pwm.range);
    gpioSetPWMfrequency(m_pin, config.pwm.freq);
    set(0.0f);
}

void PwmPin::set(float val) {
    val = std::clamp(val, 0.0f, 1.0f);
    if (m_invert) val = 1.0f - val;
    m_val = val;
    
    const auto range = gpioGetPWMrange(m_pin);
    const auto write = int(val * range);
    const auto res = gpioPWM(m_pin, write);
    pigpio::checkError(res);
}


ServoPin::ServoPin(const PinConfig& config) :
    OutputPin(config.pin, config.invert)
{
    gpioSetMode(m_pin, PI_ALT5);
    set(0.0f);
}

void ServoPin::set(float val) {
    if (m_invert) val *= -1.0f;
    m_val = val;
    
    const int write = int(val * 1000.0f) + 1500;
    const auto res = gpioServo(m_pin, write);
    pigpio::checkError(res);
}


InputPin::InputPin(const PinConfig& config) :
    Pin(config.pin),
    m_invert(config.invert)
{
    gpioSetMode(m_pin, PI_INPUT);
}

void InputPin::set(float val) {
    throw std::runtime_error("Cannot set an input pin");
}

float InputPin::get() {
    const auto read = gpioRead(m_pin);
    pigpio::checkError(read);
    auto val = float(read);
    if (m_invert) val = 1.0f - val;
    return val;
}


std::unique_ptr<Pin> Pin::create(const PinConfig& config) {
    switch (config.mode) {
    case PinMode::OUT:   return std::make_unique<OutputPin>(config);
    case PinMode::PWM:   return std::make_unique<PwmPin>   (config);
    case PinMode::SERVO: return std::make_unique<ServoPin> (config);
    case PinMode::IN:    return std::make_unique<InputPin> (config);
    default: throw std::logic_error(fmt::format("Unexpected pin mode: {}", to_underlying(config.mode)));
    }
}

} // namespace wiring
