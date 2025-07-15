#include <chrono>
#include <format>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#include "pi/Connection.hpp"
#include "pi/Input.hpp"
#include "pi/Output.hpp"
#include "utils/Duration.hpp"
#include "utils/File.hpp"
#include "utils/JsonHelper.hpp"
#include "utils/Timer.hpp"

#include "program/Base.hpp"

using namespace program;
using namespace pi;
using namespace std::literals;

class Prgm : public Base {
public:
    Prgm(std::string_view nm) : Base(nm) {
        parser.addPositional(path, "path", "The path to the json file.");

        examples.push_back(std::format("{} config.json", prgmName));
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
                connections.emplace_back(pi::parseConnection(cfg, inputs, outputs));
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
    boost::json::value json;
    Duration period = 10ms;
    std::map<std::string_view, std::unique_ptr<Input>> inputs;
    std::vector<std::function<void()>> connections;
    std::map<std::string_view, std::unique_ptr<Output>> outputs;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}
