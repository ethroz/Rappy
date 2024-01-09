#include <chrono>
#include <thread>
#include <fmt/core.h>

#include "light/Color.hpp"
#include "light/Light.hpp"
#include "light/Controller.hpp"
#include "utils/Duration.hpp"

#include "program/Base.hpp"

using namespace std::literals; 

class Prgm : public program::Base {
public:
    Prgm(std::string_view nm) : program::Base(nm) {
        parser.addPositional(name, "name", "\"RGB LED\" or \"Dual-Color LED\".");
        parser.addPositional(pins, "pins", "The output pins connected to the light.");
        parser.addPositional(mode, "mode", "cycle, solid, or flash.");
        parser.addOptional(brightness, "brightness", "The maximum brightness for the light.");
        parser.addOptional(period, "period", "The period of a full cycle or flash.");
        parser.addOptional(color, "color", "The color to use for the solid and flash modes.");

        examples.push_back(fmt::format("{} \"RGB LED\" 0 1 2 cycle --period 5s --brightness 0.5", prgmName));
        examples.push_back(fmt::format("{} \"Dual-Color LED\" 3 4 solid --color red", prgmName));
        examples.push_back(fmt::format("{} \"RGB LED\" 0 1 2 flash --color white --period 10ms", prgmName));
    }

    void init() override {
        auto lName = light::Light::nameFromString(name);
        auto cMode = light::Controller::modeFromString(mode);
        auto light = light::Light::create(lName, light::LightMode::PWM, pins);
        controller = light::Controller::create(cMode, std::move(light), color, brightness, period, out);
    }
    
    void loop() override {
        controller->step();
        std::this_thread::sleep_for(10ms);
    }

private:
    std::string name;
    std::string mode;
    std::vector<int> pins;
    float brightness = 1.0f;
    Duration period = 1.0f;
    light::Color color = light::Color::WHITE();
    std::unique_ptr<light::Controller> controller;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}

