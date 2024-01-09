#include <endian.h>
#include <pigpio.h>
#include <fmt/core.h>

#include "utils/Other.hpp"
#include "utils/PigpioError.hpp"

#include "I2cDevice.hpp"

namespace wiring {

static constexpr unsigned I2C_BUS = 1;

I2cDevice::I2cDevice(uint8_t address, bool bigEndian) : 
    m_handle(pigpio::checkError(i2cOpen(I2C_BUS, address, 0))),
    m_bigEndian(bigEndian)
{}

I2cDevice::~I2cDevice() {
    i2cClose(m_handle);
}

template <>
void I2cDevice::write(int reg, uint8_t data) const {
    const auto ret = i2cWriteByteData(m_handle, reg, data);
    pigpio::checkError(ret);
}

template <>
void I2cDevice::write(int reg, int16_t data) const {
    if (m_bigEndian) {
        data = htobe16(data);
    }
    else {
        data = htole16(data);
    }

    const auto ret = i2cWriteWordData(m_handle, reg, data);
    pigpio::checkError(ret);
}

template <>
uint8_t I2cDevice::read(int reg) const {
    const auto ret = i2cReadByteData(m_handle, reg);
    return pigpio::checkError(ret);
}

template <>
int16_t I2cDevice::read(int reg) const {
    const auto ret = i2cReadWordData(m_handle, reg);
    pigpio::checkError(ret);

    int16_t data = ret;
    
    if (m_bigEndian) {
        data = be16toh(data);
    }
    else {
        data = le16toh(data);
    }

    return data;
}

} // namespace wiring

