#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "Socket.hpp"

class Process {
public:
    Process(std::string_view command);
    ~Process() { stop(); }
    
    enum class State : uint8_t {
        STOPPED, // Not running.
        RUNNING, // Actively running.
        DEAD     // Was running.
    };

    State state() const;

    void run();
    int stop();

    bool in(std::string_view buffer, int waitFor = NO_BLOCK) const;
    std::string out(int waitFor = NO_BLOCK) const;
    std::string err(int waitFor = NO_BLOCK) const;

private:
    bool isStopped() const { return m_pid < 0; }

    using Pid = int;

    std::string m_command;
    std::string m_argumentContainer;
    std::vector<char*> m_arguments;
    Pid m_pid = -1;
    std::unique_ptr<Socket> m_in;
    std::unique_ptr<Socket> m_out;
    std::unique_ptr<Socket> m_err;
};

