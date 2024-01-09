#include <chrono>
#include <string>
#include <thread>
#include <fmt/core.h>

#include "servo/Controller.hpp"
#include "servo/Motor.hpp"
#include "utils/Duration.hpp"

#include "program/Base.hpp"

using namespace std::literals;

class Prgm : public program::Base {
public:
    Prgm(std::string_view nm) : program::Base(nm) {
        parser.addPositional(name, "name", "FS90R");
        parser.addPositional(pin, "pin", "The pin that is connected to the servo.");
        parser.addPositional(mode, "mode", "constant, oscillate, and dance");
        parser.addOptional(max, "max", "The absolute maximum power to the motor.");
        parser.addOptional(period, "period", "The period of one oscillation.");

        examples.push_back(fmt::format("{} FS90R 5 constant --max -1.0", prgmName));
        examples.push_back(fmt::format("{} FS90R 5 oscillate --max 0.25 --period 5s", prgmName));
        examples.push_back(fmt::format("{} FS90R 5 dance --period 1s", prgmName));
    }

    void init() override {
        auto sName = servo::Motor::nameFromString(name);
        auto cMode = servo::Controller::modeFromString(mode);
        auto servo = servo::Motor::create(sName, pin);
        controller = servo::Controller::create(cMode, std::move(servo), max, period);
    }
    
    void loop() override {
        controller->step();
        std::this_thread::sleep_for(1ms);
    }

private:
    int pin;
    std::string name;
    std::string mode;
    float max = 1.0f;
    Duration period = 1.0f;
    std::unique_ptr<servo::Controller> controller;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}

