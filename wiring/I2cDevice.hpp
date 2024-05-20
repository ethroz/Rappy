#pragma once

#include <cstdint>

#include "Context.hpp"

namespace wiring {

class I2cDevice {
public:
    I2cDevice(uint8_t address, bool bigEndian = false);
    ~I2cDevice();

    template <typename T>
    void write(int reg, T data) const;

    template <typename T>
    T read(int reg) const;

private:
    const Context m_contextData;
    const Context m_contextClock;
    const unsigned m_handle;
    const bool m_bigEndian;
};

} // namespace wiring

