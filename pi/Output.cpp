#include <format>
#include <map>
#include <stdexcept>

#include "light/Controller.hpp"
#include "light/Light.hpp"
#include "utils/JsonHelper.hpp"
#include "utils/Logger.hpp"
#include "utils/Other.hpp"

#include "Output.hpp"

namespace pi {

CREATE_ENUM_SET(OutputType, LIGHT, MOTOR)

std::unique_ptr<Output> Output::create(const boost::json::object& cfg) {
    const auto typeStr = getAsOrThrow<std::string_view>(cfg, "type", "pi::Output::create()");
    const auto type = OutputTypeFromString(typeStr);
    logger.debug() << "pi::Output::create(): Adding " << typeStr;
    switch (type) {
    case OutputType::LIGHT: {
        using namespace light;
        auto light = Light::create(cfg);
        return Controller::create(cfg, std::move(light));
    }
    case OutputType::MOTOR: {
        using namespace motor;
        auto motor = Motor::create(cfg);
        return Controller::create(cfg, std::move(motor));
    }
    default: throw std::invalid_argument(std::format("Unrecognized Output type: {}", typeStr));
    }
}

} // namespace pi
