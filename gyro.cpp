#include <chrono>
#include <cmath>
#include <cstdint>
#include <map>
#include <memory>
#include <thread>
#include <fmt/core.h>

#include "gyro/Gyrometer.hpp"
#include "utils/Duration.hpp"
#include "utils/Other.hpp"
#include "utils/Terminal.hpp"

#include "program/Base.hpp"

using namespace program;
using namespace gyro;
using namespace std::literals;

enum class GyroMode : uint8_t {
    ACCEL,
    GYRO
    // TODO: both
};

static const std::map<std::string_view, GyroMode> MODE_MAP = {
    {"accel", GyroMode::ACCEL},
    {"gyro",  GyroMode::GYRO }
};

GyroMode modeFromString(std::string_view mode) {
    const auto modeStr = tolower(mode);

    const auto it = MODE_MAP.find(modeStr);
    if (it == MODE_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized gyro mode: {}", modeStr));
    }

    return it->second;
}

class Prgm : public Base {
public:
    Prgm(std::string_view nm) : Base(nm) {
        parser.addPositional(
            [this](ArgPtr& argPtr, const ArgPtr end) {
                if (argPtr == end) {
                    return false;
                }
                mode = modeFromString(argPtr->data());
                argPtr++;
                return true;
            },
            "mode", "Accel or Gyro", "\"\"");
        parser.addOptional(period, "period", "The period to wait between updating the data.");
        parser.addOptional(calibrate, "calibrate", "Calibrate the gyrometer first.");
        parser.addOptional(test, "test", "Test the gyrometer against its factory trim.");
        // TODO: Add options for accel max and gyro max.

        examples.push_back(fmt::format("{} Accel --calibrate --period 10ms", prgmName));
        examples.push_back(fmt::format("{} Gyro --test", prgmName));
    }

    void init() override {
        g = std::make_unique<Gyrometer>();
       
        if (test) {
            if (mode == GyroMode::ACCEL) {
                // TODO: Fix.
                g->selfTest();
            }
            else {
                // TODO: Fix.
                g->selfTest();
            }

            running = false;
            return;
        }

        if (calibrate) {
            // TODO: Choose calibration.
            g->calibrate();
        }
    }
    
    void loop() override {
        switch (mode) {
        case GyroMode::ACCEL:
        {
            const auto accelX = g->accelX();
            const auto accelY = g->accelY();
            const auto accelZ = g->accelZ();

            const auto accelLen = std::sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);

            terminal::clear(false);
            logger.info() << fmt::format( 
                "accelX: {:.1f}\n"
                "accelY: {:.1f}\n"
                "accelZ: {:.1f}\n"
                "accel:  {:.1f}",
                accelX,
                accelY,
                accelZ,
                accelLen);
  
            break;
        }
        case GyroMode::GYRO:
        {
            const auto rotX = g->rotX();
            const auto rotY = g->rotY();
            const auto rotZ = g->rotZ();

            terminal::clear(false);
            logger.info() << fmt::format( 
                "rotX: {:.1f}\n"
                "rotY: {:.1f}\n"
                "rotZ: {:.1f}",
                rotX,
                rotY,
                rotZ);

            break;
        }
        default: throw std::logic_error(fmt::format("Unexcepted gyro mode: {}", to_underlying(mode)));
        }

        std::this_thread::sleep_for(period.ns());
    }

private:
    GyroMode mode;
    Duration period = 10ms;
    bool calibrate = false;
    bool test = false;
    std::unique_ptr<Gyrometer> g;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}

