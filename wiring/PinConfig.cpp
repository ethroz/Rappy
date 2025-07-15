#include <format>
#include <map>
#include <stdexcept>
#include <string_view>

#include "utils/Other.hpp"

#include "PinConfig.hpp"

namespace wiring {

static const std::map<std::string_view, PinMode> MODE_MAP = {
    {"in",      PinMode::IN   },
    {"out",     PinMode::OUT  },
    {"digital", PinMode::OUT  },
    {"pwm",     PinMode::PWM  },
    {"servo",   PinMode::SERVO}
};

PinMode modeFromString(std::string_view mode) {
    const auto modeStr = tolower(mode);

    const auto it = MODE_MAP.find(modeStr);
    if (it == MODE_MAP.end()) {
        throw std::invalid_argument(std::format("Unrecognized pin mode: {}", modeStr));
    }

    return it->second;
}

int pinRemap(int pin) {
    switch (pin) {
    case 0:  return 2;
    case 1:  return 3;
    case 2:  return 4;
    case 3:  return 17;
    case 4:  return 27;
    case 5:  return 22;
    case 6:  return 10;
    case 7:  return 9;
    case 8:  return 11;
    case 9:  return 0;
    case 10: return 5;
    case 11: return 6;
    case 12: return 13;
    case 13: return 19;
    case 14: return 26;
    case 15: return 21;
    case 16: return 20;
    case 17: return 16;
    case 18: return 12;
    case 19: return 1;
    case 20: return 7;
    case 21: return 8;
    case 22: return 25;
    case 23: return 24;
    case 24: return 23;
    case 25: return 18;
    case 26: return 15;
    case 27: return 14;
    default: throw std::logic_error(std::format("Pin is out of range 0-27: {}", pin));
    }
}

} // namespace wiring
