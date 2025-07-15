#include <format>
#include <stdexcept>

#include <pigpio.h>

#include "utils/PigpioError.hpp"

#include "Context.hpp"

namespace wiring {

constexpr int numPins = 28;

std::atomic_uint8_t Context::s_instances = 0;
std::atomic<Context::PinFlag> Context::s_usedPins = 0;

Context::Context(int pin) : m_pin(pin) {
    if (pin >= numPins) {
        throw std::invalid_argument(std::format("wiring::Context::Context(): Pin number ({}) exceeds number of pins ({})", m_pin, numPins));
    }

    if (s_instances++ == 0) {
        const auto res = gpioInitialise();
        pigpio::checkError(res);
    }

    PinFlag flag = 1 << m_pin;
    if ((s_usedPins & flag) > 0) {
        throw std::invalid_argument(std::format("wiring::Context::Context(): Pin is already in use: {}", m_pin));
    }
    s_usedPins |= flag;
}

Context::~Context() {
    PinFlag mask = ~(1 << m_pin);
    s_usedPins &= mask;
    
    if (--s_instances == 0) {
        gpioTerminate();
    }
}

} // namespace wiring
