#pragma once

#include <array>
#include <cstdint>

#include "wiring/I2cDevice.hpp"

namespace gyro {

enum class GyroRange : uint8_t {
    DEG_250  = 0,
    DEG_500  = 1,
    DEG_1000 = 2,
    DEG_2000 = 3,
};

enum class AccelRange : uint8_t {
    G_2  = 0,
    G_4  = 1,
    G_8  = 2,
    G_16 = 3,
};

constexpr size_t NUM_AXES = 3;
constexpr size_t NUM_DIRS = 2;

// Can only read values roughly 2000x per second.
class Gyrometer {
public:
    Gyrometer(GyroRange gyroRange = GyroRange::DEG_250, AccelRange accelRange = AccelRange::G_2);
    ~Gyrometer();

    int16_t rawRotX() const;
    int16_t rawRotY() const;
    int16_t rawRotZ() const;

    int16_t rawAccelX() const;
    int16_t rawAccelY() const;
    int16_t rawAccelZ() const;

    float rotX() const;
    float rotY() const;
    float rotZ() const;

    float accelX() const;
    float accelY() const;
    float accelZ() const;

    void calibrate();

    void selfTest() const;

private:
    const wiring::I2cDevice m_device;
    std::array<std::array<int16_t, NUM_DIRS>, NUM_AXES> m_gyroAdj;
    std::array<std::array<int16_t, NUM_DIRS>, NUM_AXES> m_accelAdj;
};

} // namespace gyro

