#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <map>
#include <memory>
#include <thread>
#include <fmt/core.h>

#include "device/Device.hpp"
#include "device/ControllerState.hpp"
#include "utils/Duration.hpp"
#include "utils/Terminal.hpp"

#include "program/Base.hpp"

using namespace program;
using namespace device;
using namespace std::literals;

class Prgm : public Base {
public:
    Prgm(std::string_view nm) : Base(nm) {
        parser.addPositional(id, "id", "The id of the device.");
        parser.addOptional(period, "period", "The period between device updates.");

        examples.push_back(fmt::format("{} js0", prgmName));
        examples.push_back(fmt::format("{} js0 --period 1ms", prgmName));
    }

    void init() override {
        device = std::make_unique<Device<ControllerState>>(id);
    }
    
    void loop() override {
        const auto state = device->read();

        terminal::clear(false);
        out << state;

        std::this_thread::sleep_for(period.ns());
    }

private:
    std::string id;
    Duration period = 10ms;
    std::unique_ptr<Device<ControllerState>> device;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}

