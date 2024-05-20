#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string_view>
#include <thread>
#include <vector>
#include <fmt/format.h>

#include "device/Device.hpp"
#include "device/ControllerState.hpp"
#include "program/Argument.hpp"
#include "servo/Motor.hpp"
#include "utils/Duration.hpp"
#include "utils/Other.hpp"
#include "utils/Terminal.hpp"

#include "program/Base.hpp"

using namespace program;
using namespace device;
using namespace servo;
using namespace std::literals;

using ControlMapFunc = std::function<float(const ControllerState& state)>;

static const std::map<std::string_view, ControlMapFunc> CONTROL_MAP = {
    {"xl", [](const ControllerState& state) { return normalize(state.xLeftJoy); }},
    {"yl", [](const ControllerState& state) { return normalize(state.yLeftJoy); }},
    {"lt", [](const ControllerState& state) { return normalize(state.leftTrigger); }},
    {"xr", [](const ControllerState& state) { return normalize(state.xRightJoy); }},
    {"yr", [](const ControllerState& state) { return normalize(state.yRightJoy); }},
    {"rt", [](const ControllerState& state) { return normalize(state.rightTrigger); }},
    {"lrt", [](const ControllerState& state) {
        return normalize(int16_t(state.rightTrigger / 2) - int16_t(state.leftTrigger / 2)); }}
};

ControlMapFunc mappingFromString(std::string_view bind) {
    const auto bindStr = tolower(bind);

    const auto& it = CONTROL_MAP.find(bindStr);
    if (it == CONTROL_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized controller binding: {}", bindStr));
    }

    return it->second;
}

class Prgm : public Base {
public:
    Prgm(std::string_view nm) : Base(nm) {
        parser.addPositional(id, "id", "The input id for the controller.");
        parser.addRequired(
            [this](ArgPtr& argPtr, const ArgPtr end) {
                if (argPtr == end) {
                    return false;
                }
                names.emplace_back(*argPtr);
                argPtr++;
                
                pins.push_back({});
                const auto ret = intListFromArgs(argPtr, end, pins.back());
                if (!ret) {
                    return false;
                }

                if (argPtr == end) {
                    return false;
                }
                binds.emplace_back(*argPtr);
                argPtr++;
                return true;
            },
            "control",
            "The name of the motor, the pins for the motor, and binding on the controller.",
            "\"\" #... \"\"");
        parser.addOptional(period, "period", "The period between device updates.");

        examples.push_back(fmt::format("{} js0 --control L298N 10 11 XL --control L298N 12 13 XR", prgmName));
        examples.push_back(fmt::format("{} js0 --control L298N 10 11 XL --period 1ms", prgmName));
    }

    void init() override {
        if (names.size() != pins.size() || names.size() != binds.size()) {
            throw std::logic_error("Assumed that there would be equal number of names, pins, and binds");
        }
        
        motors.resize(pins.size());
        mappings.resize(pins.size());
        for (size_t i = 0; i < pins.size(); i++) {
            const auto name = Motor::nameFromString(names[i]);
            motors[i] = Motor::create(name, pins[i]);
            mappings[i] = mappingFromString(binds[i]);
        }
        
        controller = std::make_unique<Device<ControllerState>>(id);
    }
    
    void loop() override {
        const auto state = controller->read();

        terminal::clear(false);
        logger.info() << state;

        for (size_t i = 0; i < motors.size(); i++) {
            const auto val = mappings[i](state); 
            motors[i]->set(val);
        }

        std::this_thread::sleep_for(period.ns());
    }

private:
    std::string id;
    std::vector<std::string> names;
    std::vector<std::vector<int>> pins;
    std::vector<std::string> binds;
    Duration period = 10ms;
    std::vector<std::unique_ptr<Motor>> motors;
    std::vector<ControlMapFunc> mappings;
    std::unique_ptr<Device<ControllerState>> controller;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}

