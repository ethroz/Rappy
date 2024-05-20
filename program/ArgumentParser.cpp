#include <cassert>
#include <stdexcept>
#include <vector>
#include <fmt/format.h>

#include "ArgumentParser.hpp"

namespace program {

//ArgMap ArgumentParser::argsToMap(std::span<char*> args) {
//    ArgMap map;
//    std::string key;
//    std::string value;
//
//    const auto addOld = [&]() {
//        if (!key.empty()) {
//            if (map.contains(key)) {
//                throw std::invalid_argument("Cannot specify the same tag more than once");
//            }
//            map[key] = std::move(value);
//            key.clear();
//            value.clear();
//        }
//    };
//
//    assert(args.size() >= 1);
//    args = args.subspan(1);
//    for (std::string arg : args) {
//        if (arg.starts_with("--")) {
//            addOld();
//            key = arg.substr(2);
//        }
//        else if (arg.starts_with('-')) {
//            addOld();
//
//            for (size_t j = 1; j < arg.size(); j++) {
//                std::string argJ = arg.substr(j,1);
//                if (map.contains(argJ)) {
//                    throw std::invalid_argument(fmt::format("Cannot specify the same flag more than once: {}", arg));
//                }
//                map[std::move(argJ)] = "";
//            }
//        }
//        else {
//            if (!value.empty()) {
//                value += ' ';
//            }
//            value.append(std::move(arg));
//        }
//    }
//
//    addOld();
//    if (!value.empty()) {
//        throw std::invalid_argument(fmt::format("Unexpected argument: {}", value));
//    }
//
//    return map;
//}

void ArgumentParser::parse(int argc, char* argv[]) const {
    std::vector<std::string_view> args;
    args.reserve(argc);
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }

    auto argPtr = args.begin();
    const auto end = args.end();

    // Parse the positional arguments.
    std::vector<bool> posPassed(m_posArgs.size());
    for (size_t i = 0; i < m_posArgs.size(); i++) {
        if (m_posArgs[i].get(argPtr, end)) {
            posPassed[i] = true;
        }
    }

    // Parse the rest of the arguments.
    std::vector<bool> passed(m_args.size());
    while (argPtr != end) {
        bool found = false;
        for (size_t i = 0; i < m_args.size(); i++) {
            if (m_args[i].get(argPtr, end)) {
                passed[i] = true;
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::invalid_argument(fmt::format("Unrecognized argument: {}", *argPtr));
        }
    }

    // Check if we missed any positional arguments.
    for (size_t i = 0; i < m_posArgs.size(); i++) {
        if (!posPassed[i]) {
            throw std::invalid_argument(fmt::format("Missing {} at argument {}", m_posArgs[i].tag(), m_posArgs[i].position()));
        }
    }

    // Check if we missed any required arguments.
    for (size_t i = 0; i < m_args.size(); i++) {
        if (m_args[i].required() && !passed[i]) {
            throw std::invalid_argument(fmt::format("Missing requried argument: {}", m_args[i].tag()));
        }
    }
}

std::string ArgumentParser::description() const {
    std::string desc;
    desc.reserve(1000);
    desc += m_programName;

    for (const auto& arg : m_posArgs) {
        desc += ' ';
        desc += arg.format();
    }

    for (const auto& arg : m_args) {
        desc += ' ';
        desc += arg.format();
    }

    for (const auto& arg : m_posArgs) {
        desc += '\n';
        desc += arg.tag();
        if (!arg.explanation().empty()) {
            desc += ": ";
            desc += arg.explanation();
        }
    }

    for (const auto& arg : m_args) {
        desc += '\n';
        desc += arg.tag();
        if (!arg.explanation().empty()) {
            desc += ": ";
            desc += arg.explanation();
        }
    }

    return desc;
}

} // namespace program

