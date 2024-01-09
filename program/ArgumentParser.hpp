#pragma once

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "Argument.hpp"

namespace program {

class ArgumentParser {
public:
    ArgumentParser(std::string_view programName) : m_programName(programName) {}

    template <typename T>
    inline
    void addPositional(T& ref, std::string_view tag, std::string_view explanation) {
        m_posArgs.push_back(Argument(ref, tag, explanation, true, m_posArgs.size() + 1));
    }

    inline
    void addPositional(ParseFunc&& func, std::string_view tag, std::string_view explanation, std::string_view format) {
        m_posArgs.push_back(Argument(tag, explanation, true, m_posArgs.size() + 1, format, std::move(func)));
    }

    template <typename T>
    inline
    void addRequired(T& ref, std::string_view tag, std::string_view explanation) {
        m_args.push_back(Argument(ref, tag, explanation, true));
    }

    inline
    void addRequired(ParseFunc&& func, std::string_view tag, std::string_view explanation, std::string_view format) {
        m_args.push_back(Argument(tag, explanation, true, Argument::NPOS, format, std::move(func)));
    }

    template <typename T>
    inline
    void addOptional(T& ref, std::string_view tag, std::string_view explanation) {
        m_args.push_back(Argument(ref, tag, explanation, false));
    }

    inline
    void addOptional(ParseFunc&& func, std::string_view tag, std::string_view explanation, std::string_view format) {
        m_args.push_back(Argument(tag, explanation, false, Argument::NPOS, format, std::move(func)));
    }

    void parse(int argc, char* argv[]) const;

    std::string description() const;

private:
    const std::string m_programName;
    std::vector<Argument> m_posArgs;
    std::vector<Argument> m_args;
};

} // namespace program

