#include <chrono>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include <dirent.h>

#include "servo/Controller.hpp"
#include "servo/Motor.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"

using namespace std::literals;

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
    
    //std::vector<std::string_view> v;
    //v.reserve(argc);
    //for (int i = 0; i < argc; i++) {
    //    v.push_back(argv[i]);
    //}

    //auto it = v.begin();
    //logger.info() << "Size of vector iterator: " << sizeof(it) << " bytes\n";

    // Get the directory path.
    const std::string dir = "/sys/class/leds/";

    // Open the directory
    DIR* dp = opendir(dir.c_str());
    if (dp == NULL) {
        logger.error() << "Invalid directory: " << dir;
        return 2;
    }

    // Iterate over the directory entries
    struct dirent* entry;
    while ((entry = readdir(dp)) != NULL) {
        std::string type;
        switch (entry->d_type) {
        case DT_REG: type = "file"; break;
        case DT_DIR: type = "dir"; break;
        default:     type = "other"; break;
        }
        logger.info() << entry->d_name << " - " << type;
    }

    // Close the directory
    closedir(dp);

    return 0;}

