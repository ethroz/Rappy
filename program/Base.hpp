#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <string_view>

#include "utils/Logger.hpp"
#include "ArgumentParser.hpp"

namespace program {

class Base {
public:
    int run(int argc, char* argv[]) noexcept;

    void help() const noexcept;

    virtual void handler(int signal);

protected:
    Base(std::string_view name) noexcept;

    virtual void init() = 0;
    virtual void loop() = 0;

    const Logger out;
    const Logger err;
    const std::string prgmName;
    std::atomic_bool running = true;
    ArgumentParser parser;

    std::vector<std::string> examples;

private:
    bool m_help = false;
};

} // namespace program

