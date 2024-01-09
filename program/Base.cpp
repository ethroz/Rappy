#include <csignal>
#include <iostream>
#include <span>
#include <stdexcept>
#include <fmt/core.h>

#include "ArgumentParser.hpp"

#include "Base.hpp"

namespace program {

static Base* ptr = nullptr;

void sigHandler(int signal) {
    ptr->handler(signal);
}

Base::Base(std::string_view name) noexcept : 
    out(std::cout), 
    err(std::cerr),
    prgmName(name),
    parser(name)
{}

void Base::handler(int signal) {
    out << " Exitting...";
    running = false;
}

int Base::run(int argc, char* argv[]) noexcept {
    try {
        // Add this after all other arguments.
        parser.addOptional(m_help, "help", "Displays the help information");

        if (ptr) {
            throw std::logic_error("Should only have one program class per program");
        }
        ptr = this;
    
        parser.parse(argc, argv);
        init();
    }
    catch (const std::exception& e) {
        if (!m_help && argc > 1) {
            err << fmt::format("Initialization error:\n\n    {}\n", e.what());
        }
        help();
        return 1;
    }

    if (m_help) {
        help();
        return 0;
    }

    std::signal(SIGINT, sigHandler);

    try {
        while (running) {
            loop();
        }
    }
    catch (const std::exception& e) {
        err << e.what();
        return 1;
    }

    return 0;
}

void Base::help() const noexcept {
    try {
        std::string message = parser.description();
        
        if (!examples.empty()) {
            message += "\n\nexample usage:";
            for (const auto& example : examples) {
                message += '\n';
                message += example;
            }
        }

        out << message;
    }
    catch (const std::exception& e) {
        err << fmt::format("Failed to generate help message: {}", e.what());
    }
}

} // namespace program

