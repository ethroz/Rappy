#include <csignal>
#include <format>
#include <iostream>
#include <span>
#include <stdexcept>

#include "utils/Other.hpp"

#include "ArgumentParser.hpp"

#include "Base.hpp"

namespace program {

static Base* ptr = nullptr;

void sigHandler(int signal) {
    ptr->handler(signal);
}

Base::Base(std::string_view name) noexcept : 
    prgmName(name),
    parser(name),
    logLevel(to_underlying(logger.logLevel()))
{}

void Base::handler(int signal) {
    logger.info() << " Exitting...";
    running = false;
}

int Base::run(int argc, char* argv[]) noexcept {
    try {
        // Add this after all other arguments.
        parser.addOptional(m_help, "help", "Displays the help information");
        parser.addOptional(logLevel, "log-level", "The verbosity of the logs.");

        if (ptr) {
            throw std::logic_error("Base::run(): Only allowed one program class per program");
        }
        ptr = this;
    
        parser.parse(argc, argv);
        logger.logLevel(LogLevel(logLevel));
        init();
    }
    catch (const std::exception& e) {
        if (!m_help && argc > 1) {
            logger.error() << std::format("Initialization error:\n\n    {}\n", e.what());
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
        logger.error() << e.what();
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

        logger.info() << message;
    }
    catch (const std::exception& e) {
        logger.error() << std::format("Base::help(): Failed to generate help message: {}", e.what());
    }
}

} // namespace program

