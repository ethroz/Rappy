#include <chrono>
#include <string_view>
#include <thread>
#include <vector>

#include "servo/Controller.hpp"
#include "servo/Motor.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"

using namespace std::literals;

Logger out;
Logger err(std::cerr);

int main(int argc, char* argv[]) {
    //for (float f = 1.0f; f > 0.0f; f /= 2.0f) {
    //    auto servo = servo::Motor::create(servo::MotorName::FS90R, 15);
    //    auto controller = servo::Controller::create(servo::ControlMode::OSCILLATE, std::move(servo), 1.0f, f);

    //    Timer timer;
    //    timer.start();
    //    while (timer.elapsed() < 2.0f) {
    //        controller->step();
    //    }
    //}
    
    std::vector<std::string_view> v;
    v.reserve(argc);
    for (int i = 0; i < argc; i++) {
        v.push_back(argv[i]);
    }

    auto it = v.begin();
    out << "Size of vector iterator: " << sizeof(it) << " bytes\n";

    return 0;
}

