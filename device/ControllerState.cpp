#include <string>
#include <vector>

#include "ControllerState.hpp"

using namespace device;

std::ostream& operator<<(std::ostream& out, const Direction& dir) {
    std::vector<std::string> strs;
    
    if ((dir & UP) > 0) {
        strs.emplace_back("up");
    }
    if ((dir & DOWN) > 0) {
        strs.emplace_back("down");
    }
    if ((dir & LEFT) > 0) {
        strs.emplace_back("left");
    }
    if ((dir & RIGHT) > 0) {
        strs.emplace_back("right");
    }

    bool first = true;
    for (const auto& str : strs) {
        if (!first) {
            out << " ";
        }
        first = false;
        out << str;
    }

    if (strs.empty()) {
        out << "None";
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, const ControllerState& state) {
    return out << 
        "X:        " << state.xButton.pressed << "\n"
        "Circle:   " << state.circleButton.pressed << "\n"
        "Triangle: " << state.triangleButton.pressed << "\n"
        "Square:   " << state.squareButton.pressed << "\n"
        "\n"
        "L1:      " << state.leftBumper.pressed << "\n"
        "R1:      " << state.rightBumper.pressed << "\n"
        "Share:   " << state.shareButton.pressed << "\n"
        "Options: " << state.optionsButton.pressed << "\n"
        "PS:      " << state.playstationButton.pressed << "\n"
        "LS:      " << state.leftStick.pressed << "\n"
        "RS:      " << state.rightStick.pressed << "\n"
        "\n"
        "L2: " << state.leftTrigger << "\n"
        "R2: " << state.rightTrigger << "\n"
        "\n"
        "Left Joy X:  " << state.xLeftJoy << "\n"
        "Left Joy Y:  " << state.yLeftJoy << "\n"
        "Right Joy X: " << state.xRightJoy << "\n"
        "Right Joy X: " << state.yRightJoy << "\n"
        "\n"
        "DPad: " << state.directionalPad;
}

