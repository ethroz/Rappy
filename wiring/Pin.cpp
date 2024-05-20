#include <algorithm>
#include <stdexcept>
#include <string>
#include <pigpio.h>
#include <fmt/format.h>

#include "utils/Logger.hpp"
#include "utils/Other.hpp"
#include "utils/PigpioError.hpp"

#include "Pin.hpp"

namespace wiring {

Pin::Pin(int pin) : m_pin(pinRemap(pin)), m_ctx(pin) {}

OutputPin::OutputPin(const PinConfig& config) :
    Pin(config.pin),
    m_invert(config.invert)
{
    gpioSetMode(m_pin, PI_OUTPUT);
    set(0.0f);
}

void OutputPin::set(float val) {
    logger.trace() << "wiring::OutputPin::set(): Value: " << val;
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
