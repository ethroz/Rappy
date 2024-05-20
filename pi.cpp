#include <chrono>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#include <fmt/format.h>

#include "pi/Input.hpp"
#include "pi/Output.hpp"
#include "utils/Duration.hpp"
#include "utils/File.hpp"
#include "utils/JsonHelper.hpp"
#include "utils/Timer.hpp"

#include "program/Base.hpp"

using namespace program;
using namespace pi;
using namespace boost;
using namespace std::literals;

std::string documentation =
R"(
Json file documentation:
An object of named devices. Example:
"alias": {
    "type": "light",
    "name": "rgb",
    "mode": "cycle",
    "pins": [0,1,2],
    "pin-mode": "pwm",
    "period": "5s",
    "brightness": 0.5
})";

class Prgm : public Base {
public:
    Prgm(std::string_view nm) : Base(nm) {
        parser.addPositional(path, "path", "The path to the json file.");

        examples.push_back(fmt::format("{} config.json", prgmName));
        examples.push_back(documentation);
    }

    void init() override {
        const auto file = readFile(path);
        logger.trace() << "Prgm::init(): File contents:\n" << file;
        json = parse(file);
        const auto& root = getAsObjectOrThrow(json, "Prgm::init()");
        logger.trace() << "Prgm::init(): Config:\n" << root;

        if (auto* v = root.if_contains("inputs")) {
            const auto& inputsCfg = getAsObjectOrThrow(*v, "Prgm::init()");
            for (const auto& [alias, config] : inputsCfg) {
                const auto& cfg = getAsObjectOrThrow(config, "Prgm::init()");
                inputs[alias] = pi::Input::create(cfg);
            }
        }

        if (auto* v = root.if_contains("outputs")) {
            const auto& outputsCfg = getAsObjectOrThrow(*v, "Prgm::init()");
            for (const auto& [alias, config] : outputsCfg) {
                const auto& cfg = getAsObjectOrThrow(config, "Prgm::init()");
                outputs[alias] = pi::Output::create(cfg);
            }
        }

        if (auto* v = root.if_contains("connections")) {
            const auto& connectCfg = getAsArrayOrThrow(*v, "Prgm::init()");
            for (const auto& config : connectCfg) {
                const auto& cfg = getAsObjectOrThrow(config, "Prgm::init()");

                const auto inputStr = getAsStringViewOrThrow(cfg, "input", "Prgm::init()");
                const auto outputStr = getAsStringViewOrThrow(cfg, "output", "Prgm::init()");

                const auto inputPeriod = inputStr.find('.');
                const auto outputPeriod = outputStr.find('.');
                if (inputPeriod == std::string_view::npos || outputPeriod == std::string_view::npos) {
                    throw std::invalid_argument("Connections must contain at least one period");
                }

                const auto input = inputStr.substr(0, inputPeriod);
                const auto output = outputStr.substr(0, outputPeriod);

                if (!inputs.contains(input)) {
                    throw std::invalid_argument(fmt::format("Invalid input: {}", input));
                }
                if (!outputs.contains(output)) {
                    throw std::invalid_argument(fmt::format("Invalid output: {}", output));
                }

                const auto inputKey = inputStr.substr(inputPeriod + 1);
                const auto outputKey = outputStr.substr(outputPeriod + 1);

                auto producer = inputs[input]->getProducer(inputKey);
                auto consumer = outputs[output]->getConsumer(outputKey);

                logger.debug() << "Prgm::init(): Adding connection: " << inputStr << " > " << outputStr;

                connections.emplace_back(
                    [producer = std::move(producer), consumer = std::move(consumer)]() {
                        consumer(producer());
                    });
            }
        }

        logger.debug() << "Prgm::init(): Inputs:\n" << [this](){
            std::stringstream out;
            for (const auto& [name, input] : inputs) {
                out << name << ": " << input->type() << '\n';
            }
            return out.str();
        }();
        logger.debug() << "Prgm::init(): Outputs:\n" << [this](){
            std::stringstream out;
            for (const auto& [name, output] : outputs) {
                out << name << ": " << output->type() << '\n';
            }
            return out.str();
        }();
    }
    
    void loop() override {
        Timer timer(true);
        logger.debug() << "Prgm::loop()";
        for (const auto& [name, input] : inputs) {
            input->poll();
        }
        for (const auto& connection : connections) {
            connection();
        }
        for (const auto& [name, output] : outputs) {
            output->step();
        }
        const nanoseconds elapsed = timer.elapsed();
        logger.trace() << "Prgm::loop(): I/O took " << elapsed.count() << "ns";
        if (elapsed < period.ns()) {
            const auto remaining = period.ns() - elapsed;
            logger.trace() << "Prgm::loop(): Sleeping for " << remaining.count() << "ns";
            std::this_thread::sleep_for(remaining);
            logger.trace() << "Prgm::loop(): Slept for " << (timer.elapsed().ns() - elapsed).count() << "ns";
            logger.trace() << "Prgm::loop(): Loop took " << timer.elapsed().ns().count() << "ns";
        }
    }

private:
    std::string path;
    json::value json;
    Duration period = 10ms;
    std::map<std::string_view, std::unique_ptr<Input>> inputs;
    std::vector<std::function<void()>> connections;
    std::map<std::string_view, std::unique_ptr<Output>> outputs;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}
