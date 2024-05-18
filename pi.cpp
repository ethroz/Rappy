#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <map>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <thread>

#include <fmt/core.h>

#include <boost/json/src.hpp>

#include "light/Controller.hpp"
#include "utils/Duration.hpp"
#include "utils/File.hpp"

#include "program/Base.hpp"

using namespace program;
using namespace boost::json;
using namespace std::literals;

std::string documentation =
R"(
Json file documentation:
An object of named devices. Example:
"alias": {
    "type": "light",
    "name": "rgb",
    "pins": [0,1,2],
    "mode": "cycle",
    "period": "5s",
    "brightness": 0.5
})";

enum class Type : uint8_t {
    LIGHT
};

static const std::map<std::string_view, Type> TYPE_MAP = {
    {"light", Type::LIGHT}
};

Type typeFromString(std::string_view mode) {
    const auto typeStr = tolower(mode);

    const auto it = TYPE_MAP.find(typeStr);
    if (it == TYPE_MAP.end()) {
        throw std::invalid_argument(fmt::format("Unrecognized type: {}", typeStr));
    }

    return it->second;
}

class Prgm : public Base {
public:
    Prgm(std::string_view nm) : Base(nm) {
        parser.addPositional(path, "path", "The path to the json file.");

        examples.push_back(fmt::format("{} config.json", prgmName));
        examples.push_back(documentation);
    }

    void init() override {
        std::string json = readFile("config.json");
        object root = parse(json).as_object();

        for (const auto& [alias, config] : root) {
            const auto type = value_to<std::string_view>(config.at("type"));
            logger.info() << "Alias: " << alias << ". Type: " << type;
        }
        // auto lName = Light::nameFromString(name);
        // auto cMode = Controller::modeFromString(mode);
        // auto light = Light::create(lName, LightMode::PWM, pins);
        // controller = Controller::create(cMode, std::move(light), color, brightness, period, out);
        // device = std::make_unique<Device<ControllerState>>(id);
    }
    
    void loop() override {
        // const auto state = device->read();

        // terminal::clear(false);
        // logger.info() << state;

        // std::this_thread::sleep_for(period.ns());
    }

private:
    std::string path;
    std::vector<light::Controller> lights;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}

