#include <cstdint>
#include <format>
#include <map>
#include <stdexcept>

#include "control/Button.hpp"
#include "device/Controller.hpp"
#include "utils/Enum.hpp"
#include "utils/JsonHelper.hpp"
#include "utils/Logger.hpp"
#include "utils/Other.hpp"

#include "Input.hpp"

using namespace control;
using namespace device;

namespace pi {

CREATE_ENUM_SET(InputType, BUTTON, CONTROLLER)

std::unique_ptr<Input> Input::create(const boost::json::object& cfg) {
    const auto typeStr = getAsOrThrow<std::string_view>(cfg, "type", "pi::Input::create()");
    const auto type = InputTypeFromString(typeStr);
    logger.debug() << "pi::Input::create(): Adding " << typeStr;
    switch (type) {
    case InputType::BUTTON: {
        const auto pin = getAsOrThrow<int>(cfg, "pin", "pi::Input::create()");
        const auto toggle = getAsOr<bool>(cfg, "toggle", false);
        return std::make_unique<Button>(pin, toggle);
    }
    case InputType::CONTROLLER: {
        const auto id = getAsOrThrow<std::string_view>(cfg, "id", "pi::Input::create()");
        return std::make_unique<Controller>(id);
    }
    default: throw std::invalid_argument(std::format("Unrecognized Input type: {}", typeStr));
    }
}

} // namespace pi
