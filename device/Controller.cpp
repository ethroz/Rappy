#include <cstdint>
#include <limits>
#include <type_traits>

#include <fcntl.h>
#include <unistd.h>

#include <fmt/format.h>

#include "utils/Enum.hpp"
#include "utils/Other.hpp"

#include "ControllerState.hpp"

#include "Controller.hpp"

namespace device {

Controller::Controller(std::string_view id) : 
    pi::Input(id),
    m_socket(open(fmt::format("/dev/input/{}", id).c_str(), O_RDONLY))
{}

enum ControllerEventType : uint8_t {
    BUTTON = 1,
    AXIS = 2
};

enum ControllerButtonId : uint8_t {
    X = 0,
    CIRCLE = 1,
    TRIANGLE = 2,
    SQUARE = 3,
    LEFT_BUMPER = 4,
    RIGHT_BUMPER = 5,
    LEFT_TRIGGER_BUTTON = 6,
    RIGHT_TRIGGER_BUTTON = 7,
    SHARE = 8,
    OPTIONS = 9,
    PS = 10,
    LEFT_STICK = 11,
    RIGHT_STICK = 12
};

enum ControllerAxisId : uint8_t {
    X_LEFT_JOY = 0,
    Y_LEFT_JOY = 1,
    LEFT_TRIGGER = 2,
    X_RIGHT_JOY = 3,
    Y_RIGHT_JOY = 4,
    RIGHT_TRIGGER = 5,
    X_DPAD = 6,
    Y_DPAD = 7
};

enum ControllerButtonState : uint16_t {
    RELEASED = 0,
    PRESSED = 1
};

struct ControllerEvent {
public:
    ControllerEvent() {}

    uint32_t timestamp() const { return (time3 << 24) | (time2 << 16) | (time1 << 8) | time0; }
    uint16_t data() const { return (data1 << 8) | data0; }
    uint8_t  type() const { return m_type & 0x7F; }
    uint8_t  id() const { return m_id; }

private:
    uint8_t time0 = 0;
    uint8_t time1 = 0;
    uint8_t time2 = 0;
    uint8_t time3 = 0;
    uint8_t data0 = 0;
    uint8_t data1 = 0;
    uint8_t m_type = 0;
    uint8_t m_id = 0;
};
static_assert(std::is_trivially_copyable_v<ControllerEvent>);

void Controller::poll() {
    const auto buffer = m_socket.read();

    if (buffer.size() % sizeof(ControllerEvent) != 0) {
        throw std::logic_error("Assumed controller events are 8 bytes");
    }
    const std::span<const ControllerEvent> events(
        reinterpret_cast<const ControllerEvent*>(buffer.data()),
        buffer.size() / sizeof(ControllerEvent));
    
    for (const auto& event : events) {
        switch (event.type()) {
        case BUTTON:
        {
            ButtonState* button = nullptr;
            switch (event.id()) {
            case X:            button = &m_state.xButton; break;
            case CIRCLE:       button = &m_state.circleButton; break;
            case TRIANGLE:     button = &m_state.triangleButton; break;
            case SQUARE:       button = &m_state.squareButton; break;
            case LEFT_BUMPER:  button = &m_state.leftBumper; break;
            case RIGHT_BUMPER: button = &m_state.rightBumper; break;
            case LEFT_TRIGGER_BUTTON:
            case RIGHT_TRIGGER_BUTTON: break;
            case SHARE:        button = &m_state.shareButton; break;
            case OPTIONS:      button = &m_state.optionsButton; break;
            case PS:           button = &m_state.playstationButton; break;
            case LEFT_STICK:   button = &m_state.leftStick; break;
            case RIGHT_STICK:  button = &m_state.rightStick; break;
            default: throw std::logic_error(fmt::format("Unknown controller button id: {}", event.id()));
            }
            
            if (button != nullptr) {
                bool pressed;
                switch (event.data()) {
                case PRESSED:  pressed = true; break;
                case RELEASED: pressed = false; break;
                default: throw std::logic_error(fmt::format("Unknown controller data"));
                }
                button->pressed = pressed;
            }
            break;
        }
        case AXIS:
        {
            const auto data = Axis(event.data());
            switch (event.id()) {
            case X_LEFT_JOY:    m_state.xLeftJoy = data; break;
            case Y_LEFT_JOY:    m_state.yLeftJoy = -data; break;
            case LEFT_TRIGGER:  m_state.leftTrigger = uAxis(data + std::numeric_limits<int16_t>::max()); break;
            case X_RIGHT_JOY:   m_state.xRightJoy = data; break;
            case Y_RIGHT_JOY:   m_state.yRightJoy = -data; break;
            case RIGHT_TRIGGER: m_state.rightTrigger = uAxis(data + std::numeric_limits<int16_t>::max()); break;
            case X_DPAD:
            {
                const auto sData = int16_t(data);
                uint8_t dir = NONE;
                if (sData < 0) {
                    dir |= LEFT;
                }
                else if (sData > 0) {
                    dir |= RIGHT;
                }

                m_state.directionalPad = Direction(dir | (m_state.directionalPad & (UP | DOWN)));
                break;
            }
            case Y_DPAD:
            {
                const auto sData = int16_t(data);
                uint8_t dir = NONE;
                if (sData < 0) {
                    dir |= UP;
                }
                else if (sData > 0) {
                    dir |= DOWN;
                }

                m_state.directionalPad = Direction(dir | (m_state.directionalPad & (LEFT | RIGHT)));
                break;
            }
            default: throw std::logic_error(fmt::format("Unknown controller axis id: {}", event.id()));
            }
            break;
        }
        default: throw std::logic_error(fmt::format("Unknown controller event type: {}", event.type()));
        }
    }
}

CREATE_ENUM(ControllerBind, X, CIRCLE, TRIANGLE, SQUARE, LB, RB, SHARE, OPTIONS, PS, LSTICK, RSTICK, LRT, LT, RT, LJOY, RJOY, DPAD)
CREATE_ENUM(ControllerJoy, X, Y, LEFT, UP, RIGHT, DOWN)

pi::Producer Controller::getProducer(std::string_view key) const {
    const auto period = key.find('.');
    auto bindStr = key.substr(0, period);
    const auto bind = ControllerBindFromString(bindStr);

    const ButtonState* button = nullptr;
    switch (bind) {
    case ControllerBind::X:        button = &m_state.xButton; break;
    case ControllerBind::CIRCLE:   button = &m_state.circleButton; break;
    case ControllerBind::TRIANGLE: button = &m_state.triangleButton; break;
    case ControllerBind::SQUARE:   button = &m_state.squareButton; break;
    case ControllerBind::LB:       button = &m_state.leftBumper; break;
    case ControllerBind::RB:       button = &m_state.rightBumper; break;
    case ControllerBind::SHARE:    button = &m_state.shareButton; break;
    case ControllerBind::OPTIONS:  button = &m_state.optionsButton; break;
    case ControllerBind::PS:       button = &m_state.playstationButton; break;
    case ControllerBind::LSTICK:   button = &m_state.leftStick; break;
    case ControllerBind::RSTICK:   button = &m_state.rightStick; break;
    default: break;
    }

    // Handle all the buttons.
    if (button != nullptr) {
        return [button]() { return button->pressed ? 1.0f : 0.0f; };
    }

    // Handle the left and right trigger case.
    if (bind == ControllerBind::LRT) {
        return [this]() { return normalize(Axis(m_state.rightTrigger / 2) - Axis(m_state.leftTrigger / 2)); };
    }

    const uAxis* axis = nullptr;
    switch (bind) {
    case ControllerBind::LT: axis = &m_state.leftTrigger; break;
    case ControllerBind::RT: axis = &m_state.rightTrigger; break;
    default: break;
    }

    // Handle the individual triggers.
    if (axis != nullptr) {
        return [axis]() { return normalize(*axis); };
    }

    if (period == std::string_view::npos) {
        throw std::invalid_argument("All axis binds must contain at least one period");
    }
    const auto valueStr = tolower(key.substr(period + 1));
    const auto value = ControllerJoyFromString(valueStr);

    const Axis* axes = nullptr;
    switch (bind) {
    case ControllerBind::LJOY: axes = &m_state.xLeftJoy; break;
    case ControllerBind::RJOY: axes = &m_state.xRightJoy; break;
    default: break;
    }

    // Handle all the joysticks.
    if (axes != nullptr) {
        switch (value) {
        case ControllerJoy::X:     return [axes]() { return normalize(axes[0]); };
        case ControllerJoy::Y:     return [axes]() { return normalize(axes[1]); };
        case ControllerJoy::LEFT:  return [axes]() { return normalize(axes[0] < 0 ? -axes[0] : Axis(0)); };
        case ControllerJoy::UP:    return [axes]() { return normalize(axes[1] > 0 ?  axes[1] : Axis(0)); };
        case ControllerJoy::RIGHT: return [axes]() { return normalize(axes[0] > 0 ?  axes[0] : Axis(0)); };
        case ControllerJoy::DOWN:  return [axes]() { return normalize(axes[1] < 0 ? -axes[1] : Axis(0)); };
        default: throw std::invalid_argument(fmt::format("Unrecognized controller joystick bind: {}", valueStr));
        }
    }
    // Handle the directional pad.
    else if (bind == ControllerBind::DPAD) {        
        switch (value) {
        case ControllerJoy::X:     return [this]() { return float(int((m_state.directionalPad & RIGHT) > 0) - int((m_state.directionalPad & LEFT) > 0)); };
        case ControllerJoy::Y:     return [this]() { return float(int((m_state.directionalPad & UP   ) > 0) - int((m_state.directionalPad & DOWN) > 0)); };
        case ControllerJoy::LEFT:  return [this]() { return float((m_state.directionalPad & LEFT ) > 0); };
        case ControllerJoy::UP:    return [this]() { return float((m_state.directionalPad & UP   ) > 0); };
        case ControllerJoy::RIGHT: return [this]() { return float((m_state.directionalPad & RIGHT) > 0); };
        case ControllerJoy::DOWN:  return [this]() { return float((m_state.directionalPad & DOWN ) > 0); };
        default: throw std::invalid_argument(fmt::format("Unrecognized controller directional pad bind: {}", valueStr));
        }
    }
    else {
        throw std::invalid_argument(fmt::format("Unrecognized controller bind: {}", key));
    }
}

} // namespace device
