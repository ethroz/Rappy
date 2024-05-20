#include <cstdint>
#include <map>
#include <stdexcept>

#include <fmt/format.h>

#include "control/Button.hpp"
#include "device/Controller.hpp"
#include "utils/JsonHelper.hpp"
#include "utils/Logger.hpp"
#include "utils/Other.hpp"

#include "Input.hpp"

using namespace control;
using namespace device;

namespace pi {

enum class InputType : uint8_t {
    BUTTON,
    CONTROLLER
};

static const std::map<std::string_view, InputType> TYPE_MAP = {
    {"button",     InputType::BUTTON    },
    {"controller", InputType::CONTROLLER}
};

InputType inputTypeFromString(std::string_view mode) {
    const auto typeStr = tolower(mode);

    const auto it = TYPE_MAP.find(typeStr);
    if (it == TYPE_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized type: {}", typeStr));
    }

    return it->second;
}

std::unique_ptr<Input> Input::create(const json::object& cfg) {
    const auto typeStr = getAsStringViewOrThrow(cfg, "type", "pi::Input::create()");
    const auto type = inputTypeFromString(typeStr);
    logger.debug() << "pi::Input::create(): Adding " << typeStr;
    switch (type) {
    case InputType::BUTTON: {
        const auto pin = getAsIntOrThrow(cfg, "pin", "pi::Input::create()");
        const auto toggle = getAsBoolOr(cfg, "toggle", false);
        return std::make_unique<Button>(pin, toggle);
    }
    case InputType::CONTROLLER: {
        const auto id = getAsStringViewOrThrow(cfg, "id", "pi::Input::create()");
        return std::make_unique<Controller>(id);
    }
    default: throw std::invalid_argument(fmt::format("Unrecognized Input type: {}", typeStr));
    }
}

} // namespace pi
