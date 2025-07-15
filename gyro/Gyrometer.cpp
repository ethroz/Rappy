#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <format>
#include <limits>
#include <thread>

#include "utils/Logger.hpp"
#include "utils/Other.hpp"
#include "utils/Terminal.hpp"

#include "Gyrometer.hpp"

using namespace std::literals;

namespace gyro {

constexpr size_t X = 0;
constexpr size_t Y = 1;
constexpr size_t Z = 2;
constexpr size_t DOWN = 0;
constexpr size_t UP = 1;

constexpr std::array<std::array<int16_t, NUM_DIRS>, NUM_AXES> DEFAULT_GYRO = {{
    {{ -131, 131 }},
    {{ -131, 131 }},
    {{ -131, 131 }}
}};

constexpr std::array<std::array<int16_t, NUM_DIRS>, NUM_AXES> DEFAULT_ACCEL = {{
    {{ 16640, -16128 }},
    {{ 16128, -16640 }},
    {{ 15360, -17664 }}
}};

constexpr float remap(int16_t value, int16_t min, int16_t max) {
    return 2.0f * (value - min) / (max - min) - 1.0f;
}

Gyrometer::Gyrometer(GyroRange gyroRange, AccelRange accelRange) : 
    m_device(0x68, true)
{
    for (size_t i = 0; i < NUM_AXES; i++) {
        for (size_t j = 0; j < NUM_DIRS; j++) {
            m_gyroAdj[i][j] = DEFAULT_GYRO[i][j];
            m_accelAdj[i][j] = DEFAULT_ACCEL[i][j] >> to_underlying(accelRange);
        }
    }

    // Disable sleep mode.
    m_device.write<uint8_t>(0x6B, 0);

    // Set the ranges.
    m_device.write<uint8_t>(0x1B, to_underlying(gyroRange) << 3);
    m_device.write<uint8_t>(0x1C, to_underlying(accelRange) << 3);
}

Gyrometer::~Gyrometer() {
    // Enable sleep mode.
    m_device.write<uint8_t>(0x6B, 0x40);
}

int16_t Gyrometer::rawRotX() const {
    return m_device.read<int16_t>(0x43);
}

int16_t Gyrometer::rawRotY() const {
    return m_device.read<int16_t>(0x45);
}

int16_t Gyrometer::rawRotZ() const {
    return m_device.read<int16_t>(0x47);
}

int16_t Gyrometer::rawAccelX() const {
    return m_device.read<int16_t>(0x3B);
}

int16_t Gyrometer::rawAccelY() const {
    return m_device.read<int16_t>(0x3D);
}

int16_t Gyrometer::rawAccelZ() const {
    return m_device.read<int16_t>(0x3F);
}

float Gyrometer::rotX() const {
    return remap(rawRotX(), m_gyroAdj[X][DOWN], m_gyroAdj[X][UP]);
}

float Gyrometer::rotY() const {
    return remap(rawRotY(), m_gyroAdj[Y][DOWN], m_gyroAdj[Y][UP]);
}

float Gyrometer::rotZ() const {
    return remap(rawRotZ(), m_gyroAdj[Z][DOWN], m_gyroAdj[Z][UP]);
}

float Gyrometer::accelX() const {
    return remap(rawAccelX(), m_accelAdj[X][DOWN], m_accelAdj[X][UP]);
}

float Gyrometer::accelY() const {
    return remap(rawAccelY(), m_accelAdj[Y][DOWN], m_accelAdj[Y][UP]);
}

float Gyrometer::accelZ() const {
    return remap(rawAccelZ(), m_accelAdj[Z][DOWN], m_accelAdj[Z][UP]);
}

void Gyrometer::calibrate() {
    // Hold the accelerometer in each of the 6 directions.

    std::array<int16_t, NUM_AXES> readings;
    for (int a = NUM_AXES - 1; a >= 0; a--) {
        
        char axisChar;
        switch (a) {
        case X: axisChar = 'X'; break;
        case Y: axisChar = 'Y'; break;
        case Z: axisChar = 'Z'; break;
        default: throw std::logic_error("Unexpected axis");
        }

        for (size_t d = 0; d < NUM_DIRS; d++) {
            char dirChar;
            switch (d) {
            case DOWN: dirChar = '-'; break;
            case UP:   dirChar = '+'; break;
            default: throw std::logic_error("Unexpected direction");
            }

            terminal::clear(false);
            logger.info() << std::format("Press enter to start calibrating Accel {}{}.", dirChar, axisChar);
            std::cin.get();

            while (true) {
                readings[X] = rawAccelX();
                readings[Y] = rawAccelY();
                readings[Z] = rawAccelZ();

                terminal::clear(false);
                logger.info() <<
                    std::format("Calibrating Accel {}{}", dirChar, axisChar) << "\n"
                    "accelX: " << readings[X] << "\n"
                    "accelY: " << readings[Y] << "\n"
                    "accelZ: " << readings[Z];

                int16_t absSum = 0;
                for (int i = 0; i < 3; i++) {
                    if (i == a) {
                        continue;
                    }

                    auto& r = readings[i];
                    if (r < 0) {
                        absSum -= r;
                    }
                    else {
                        absSum += r;
                    }
                }

                if (absSum == 0) {
                    break;
                }

                std::this_thread::sleep_for(100ms);
            }

            m_accelAdj[a][d] = readings[a];
        }
    }

    logger.info() <<
        std::format("accelX: [{}, {}]\n", m_accelAdj[X][DOWN], m_accelAdj[X][UP]) <<
        std::format("accelY: [{}, {}]\n", m_accelAdj[Y][DOWN], m_accelAdj[Y][UP]) <<
        std::format("accelZ: [{}, {}]",   m_accelAdj[Z][DOWN], m_accelAdj[Z][UP]);
}

void Gyrometer::selfTest() const {
    logger.info() << "Please make sure gyrometer is still and do not touch it.\nPress enter to start test.";
    std::cin.get();
    
    m_device.write<uint8_t>(0x1C, 0xE0);

    std::this_thread::sleep_for(5ms);

    constexpr auto size = 2000;
    std::array<int16_t, size * 3> testData = {};

    for (size_t i = 0; i < size; i++) {
        testData[3 * i + 0] = rawAccelX();
        testData[3 * i + 1] = rawAccelY();
        testData[3 * i + 2] = rawAccelZ();
    }

    m_device.write<uint8_t>(0x1C, 0);

    std::this_thread::sleep_for(10ms);

    std::array<int16_t, size * 3> data = {};

    for (size_t i = 0; i < size; i++) {
        data[3 * i + 0] = rawAccelX();
        data[3 * i + 1] = rawAccelY();
        data[3 * i + 2] = rawAccelZ();
    }

    constexpr auto min = std::numeric_limits<int16_t>::min();
    constexpr auto max = std::numeric_limits<int16_t>::max();
    int16_t maxX = min; int16_t minX = max; int32_t avgX = 0;
    int16_t maxY = min; int16_t minY = max; int32_t avgY = 0;
    int16_t maxZ = min; int16_t minZ = max; int32_t avgZ = 0;

    for (size_t i = 0; i < size; i++) {
        maxX = std::max(maxX, testData[3 * i]);
        minX = std::min(minX, testData[3 * i]);
        avgX += testData[3 * i];
        maxY = std::max(maxY, testData[3 * i + 1]);
        minY = std::min(minY, testData[3 * i + 1]);
        avgY += testData[3 * i + 1];
        maxZ = std::max(maxZ, testData[3 * i + 2]);
        minZ = std::min(minZ, testData[3 * i + 2]);
        avgZ += testData[3 * i + 2];
    }

    avgX /= size;
    avgY /= size;
    avgZ /= size;

    int16_t testAvgX = avgX;
    int16_t testAvgY = avgY;
    int16_t testAvgZ = avgZ;

    logger.info() << "Test complete!";

    logger.info() << "Test data: [min, avg, max]";
    logger.info() << std::format("x: [{}, {}, {}]", minX, avgX, maxX);
    logger.info() << std::format("y: [{}, {}, {}]", minY, avgY, maxY);
    logger.info() << std::format("z: [{}, {}, {}]", minZ, avgZ, maxZ);

    maxX = min; minX = max; avgX = 0;
    maxY = min; minY = max; avgY = 0;
    maxZ = min; minZ = max; avgZ = 0;

    for (size_t i = 0; i < size; i++) {
        maxX = std::max(maxX, data[3 * i]);
        minX = std::min(minX, data[3 * i]);
        avgX += data[3 * i];
        maxY = std::max(maxY, data[3 * i + 1]);
        minY = std::min(minY, data[3 * i + 1]);
        avgY += data[3 * i + 1];
        maxZ = std::max(maxZ, data[3 * i + 2]);
        minZ = std::min(minZ, data[3 * i + 2]);
        avgZ += data[3 * i + 2];
    }

    avgX /= size;
    avgY /= size;
    avgZ /= size;

    logger.info() << "Real data:";
    logger.info() << std::format("x: [{}, {}, {}]", minX, avgX, maxX);
    logger.info() << std::format("y: [{}, {}, {}]", minY, avgY, maxY);
    logger.info() << std::format("z: [{}, {}, {}]", minZ, avgZ, maxZ);
   
    const auto regXa = m_device.read<uint8_t>(0xD);
    const auto regYa = m_device.read<uint8_t>(0xE); 
    const auto regZa = m_device.read<uint8_t>(0xF); 
    const auto regTest = m_device.read<uint8_t>(0x10);

    int16_t xaTest = ((regXa >> 3) & 0x1C) | ((regTest >> 4) & 0x3);
    int16_t yaTest = ((regYa >> 3) & 0x1C) | ((regTest >> 2) & 0x3);
    int16_t zaTest = ((regZa >> 3) & 0x1C) | (regTest & 0x3);

    logger.info() << std::format("test: [{}, {}, {}]", xaTest, yaTest, zaTest);

    float ftXa = 0.0f;
    if (xaTest > 0) {
        ftXa = 4096.0f * 0.34f * powf(0.92f / 0.34f, (xaTest - 1.0f) / 30.0f);
    }
    float ftYa = 0.0f;
    if (yaTest > 0) {
        ftYa = 4096.0f * 0.34f * powf(0.92f / 0.34f, (yaTest - 1.0f) / 30.0f);
    }
    float ftZa = 0.0f;
    if (zaTest > 0) {
        ftZa = 4096.0f * 0.34f * powf(0.92f / 0.34f, (zaTest - 1.0f) / 30.0f);
    }

    logger.info() << std::format("FT: [{}, {}, {}]", ftXa, ftYa, ftZa);

    const auto changeX = ((testAvgX - avgX) - ftXa) / ftXa;
    const auto changeY = ((testAvgY - avgY) - ftYa) / ftYa;
    const auto changeZ = ((testAvgZ - avgZ) - ftZa) / ftZa;
    logger.info() << std::format("Change: [{}, {}, {}]", changeX, changeY, changeZ);
    
    logger.info() << "AccelX: " << (abs(changeX) <= 14.0f ? "pass" : "fail");
    logger.info() << "AccelY: " << (abs(changeY) <= 14.0f ? "pass" : "fail");
    logger.info() << "AccelZ: " << (abs(changeZ) <= 14.0f ? "pass" : "fail");
}

} // namespace gyro

