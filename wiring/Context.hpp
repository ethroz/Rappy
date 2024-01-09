#pragma once

#include <atomic>

namespace wiring {

class Context {
public:
    Context();
    ~Context();

private:
    static std::atomic_uint8_t s_instances;
};

} // namespace wiring

