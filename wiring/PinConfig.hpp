#pragma once

namespace wiring {

enum class PinMode : uint8_t {
    NONE,
    IN,
    OUT,
    PWM,
    SERVO
};

struct PwmConfig {    
    // Default sample rate is 5 us.
    // The frequency will be snapped to the closest of the following:
    // 8000, 4000, 2000, 1600, 1000, 800, 500, 400, 320, 250, 200, 160, 100, 80, 50, 40, 20, 10
    int range = 255;
    int freq = 1000;
};

struct PinConfig {
    int pin = -1;
    PinMode mode = PinMode::NONE;
    bool invert = false;
    PwmConfig pwm = {};
};

} // namespace wiring

