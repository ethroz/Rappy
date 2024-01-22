#pragma once

#include <cstdint>
#include <iostream>

namespace device {

enum Direction : uint8_t {
    NONE = 0,
    UP = 0x1,
    LEFT = 0x2,
    DOWN = 0x4,
    RIGHT = 0x8,
    UP_LEFT = UP | LEFT,
    UP_RIGHT = UP | RIGHT,
    DOWN_LEFT = DOWN | LEFT,
    DOWN_RIGHT = DOWN | RIGHT
};

struct ButtonState {
    bool pressed = false;
    bool down = false;
};

using Axis = int16_t;
using uAxis = uint16_t;

struct ControllerState {
public:
    ControllerState() {}

    ButtonState xButton;
    ButtonState circleButton;
    ButtonState triangleButton;
    ButtonState squareButton;
    
    ButtonState leftBumper;
    ButtonState rightBumper;
    
    ButtonState shareButton;
    ButtonState optionsButton;
    ButtonState playstationButton;
    
    ButtonState leftStick;
    ButtonState rightStick;

    Direction directionalPad;
 
    uAxis leftTrigger = 0;
    uAxis rightTrigger = 0;
    
    Axis xLeftJoy = 0;
    Axis yLeftJoy = 0;
    Axis xRightJoy = 0;
    Axis yRightJoy = 0;
};

} // namespace device

std::ostream& operator<<(std::ostream& out, const device::Direction& dir);
std::ostream& operator<<(std::ostream& out, const device::ControllerState& state);


