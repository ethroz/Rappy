#pragma once

#include <atomic>

namespace wiring {

class Context {
public:
    Context(int pin);
    ~Context();

    using PinFlag = uint32_t;

private:

    static std::atomic_uint8_t s_instances;
    static std::atomic<PinFlag> s_usedPins;

    const int m_pin;
};

} // namespace wiring
