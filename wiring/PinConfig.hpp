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
    // The frequency will be snapped to the closest acceptable value for the given sample rate.
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

