#include <map>
#include <stdexcept>

#include <fmt/format.h>

#include "light/Controller.hpp"
#include "light/Light.hpp"
#include "utils/JsonHelper.hpp"
#include "utils/Logger.hpp"
#include "utils/Other.hpp"

#include "Output.hpp"

using namespace light;

namespace pi {

enum class OutputType : uint8_t {
    LIGHT
};

static const std::map<std::string_view, OutputType> TYPE_MAP = {
    {"light", OutputType::LIGHT}
};

OutputType outputTypeFromString(std::string_view mode) {
    const auto typeStr = tolower(mode);

    const auto it = TYPE_MAP.find(typeStr);
    if (it == TYPE_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized type: {}", typeStr));
    }

    return it->second;
}

std::unique_ptr<Output> Output::create(const json::object& cfg) {
    const auto typeStr = getAsStringViewOrThrow(cfg, "type", "pi::Output::create()");
    const auto type = outputTypeFromString(typeStr);
    logger.debug() << "pi::Output::create(): Adding " << typeStr;
    switch (type) {
    case OutputType::LIGHT: {
        auto light = Light::create(cfg);
        return Controller::create(cfg, std::move(light));
    }
    default: throw std::invalid_argument(fmt::format("Unrecognized Output type: {}", typeStr));
    }
}

} // namespace pi
