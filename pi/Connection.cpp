#include <cassert>
#include <format>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "script/Parser.hpp"
#include "utils/JsonHelper.hpp"
#include "utils/Logger.hpp"

#include "Connection.hpp"

namespace pi {

auto split(std::string_view bind) {
    const auto period = bind.find('.');
    if (period == std::string_view::npos) {
        throw std::invalid_argument("Connections must contain at least one period");
    }
    return std::make_pair(bind.substr(0, period), bind.substr(period + 1));
}

Connection parseConnection(
    const boost::json::object& cfg,
    const std::map<std::string_view, std::unique_ptr<Input>>& inputs,
    const std::map<std::string_view, std::unique_ptr<Output>>& outputs)
{
    std::vector<std::pair<std::string_view, pi::Producer>> producers;
    pi::Consumer consumer;
    std::string funcStr;
    for (const auto& [k, v] : cfg) {
        const auto str = getAsOrThrow<std::string_view>(v, "pi::parseConnections()");
        if (k == "function") {
            funcStr = str;
            continue;
        }
        const auto [bind, bindKey] = split(str);
        if (k == "output") {
            consumer = outputs.at(bind)->getConsumer(bindKey);
        }
        else {
            producers.emplace_back(k, inputs.at(bind)->getProducer(bindKey));
        }
    }
    
    // Parameter validation.
    if (producers.empty()) {
        throw std::invalid_argument("Must provide at least one input in a connection");
    }
    if (!consumer) {
        throw std::invalid_argument("Must provide one output in a connection");
    }
    if (producers.size() > 1 && funcStr.empty()) {
        throw std::invalid_argument("Must provide a function for multiple inputs in a connection");
    }

    // No function case.
    if (funcStr.empty()) {
        return [
            producer = std::move(producers[0].second),
            consumer = std::move(consumer)
            ]
            () {
                consumer(producer());
            };
    }

    // Function preparation.
    std::stringstream funcDef;
    funcDef << '(';
    bool first = true;
    for (const auto& [var, _] : producers) {
        if (!first) {
            funcDef << ',';
        }
        first = false;
        funcDef << var;
    }
    funcDef << ')' << '{' << funcStr << '}';

    if (producers.size() == 1) {
        auto func = script::parse<float>(funcDef.str());
        return [
            producer = std::move(producers[0].second),
            func = std::move(func),
            consumer = std::move(consumer)
            ]
            () {
                consumer(func(producer()));
            };
    }
    else if (producers.size() == 2) {
        auto func = script::parse<float,float>(funcDef.str());
        return [
            x = std::move(producers[0].second),
            y = std::move(producers[1].second),
            func = std::move(func),
            consumer = std::move(consumer)
            ]
            () {
                consumer(func(x(), y()));
            };
    }
    else if (producers.size() == 3) {
        auto func = script::parse<float,float,float>(funcDef.str());
        return [
            x = std::move(producers[0].second),
            y = std::move(producers[1].second),
            z = std::move(producers[2].second),
            func = std::move(func),
            consumer = std::move(consumer)
            ]
            () {
                consumer(func(x(), y(), z()));
            };
    }
    else {
        throw std::logic_error("More than two inputs to a connection is not supported");
    }
}

} // namespace pi
