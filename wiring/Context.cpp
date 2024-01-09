#include <stdexcept>
#include <pigpio.h>

#include "utils/PigpioError.hpp"

#include "Context.hpp"

namespace wiring {

std::atomic_uint8_t Context::s_instances = 0;

Context::Context() {
    if (s_instances++ == 0) {
        const auto res = gpioInitialise();
        pigpio::checkError(res);
    }
}

Context::~Context() {
    if (--s_instances == 0) {
        gpioTerminate();
    }
}

} // namespace wiring

