#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <format>
#include <iostream>
#include <stack>
#include <system_error>
#include <type_traits>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Other.hpp"
#include "Process.hpp"

constexpr auto READ = 0;
constexpr auto WRITE = 1;
constexpr auto FAIL_CODE = 127;

Process::Process(std::string_view command) : m_command(command), m_argumentContainer(command) {
    if (command.empty()) {
        throw std::invalid_argument("Empty command");
    }
    
    m_argumentContainer.resize(command.size() + 1);
    m_argumentContainer.back() = '\0';

    std::stack<char> cStack;
    for (auto& c : m_argumentContainer) {
        switch (c) {
        case ' ':
            if (cStack.empty()) {
                c = '\0';
            }
            break;
        case '\'':
        case '"':
            if (cStack.empty()) {
                cStack.push(c);
                c = '\0';
            }
            else if (cStack.top() != c) {
                cStack.push(c);
            }
            else {
                cStack.pop();
            }
            break;
        }
    }

    if (!cStack.empty()) {
        throw std::invalid_argument(std::format("Bad command format: {}", m_command));
    }

    // Split the command into its arguments.
    for (size_t i = 0; i < m_argumentContainer.size(); i = m_argumentContainer.find('\0', i)) {
        i = m_argumentContainer.find_first_not_of('\0', i);
        m_arguments.push_back(m_argumentContainer.data() + i);
    }

    m_arguments.push_back(nullptr);
}

Process::State Process::state() const {
    if (isStopped()) {
        return State::STOPPED;
    }
    else if (m_out->isClosed()) {
        return State::DEAD;
    }
    else {
        return State::RUNNING;
    }
}

void Process::run() {
    switch (state()) {
    case State::RUNNING: return;
    case State::DEAD: stop(); break;
    case State::STOPPED: break;
    }

    int inPipe[2], outPipe[2], errPipe[2];
    if (pipe(inPipe) != 0 || pipe(outPipe) != 0 || pipe(errPipe) != 0) {
        throw std::system_error(errno, std::generic_category(), "pipe");
    }

    // Open a gate to the upside down.
    static_assert(std::is_same_v<Pid, pid_t>);
    m_pid = fork();
    if (m_pid < 0) {
        // Failed to open the gate.
        throw std::system_error(errno, std::generic_category(), "fork");
    }
    
    // Which version of Hawkins are we in?
    if (m_pid == 0) {
        // In the upside down, everything is... upside down.
        close(inPipe[WRITE]);
        close(outPipe[READ]);
        close(errPipe[READ]);
        
        dup2(inPipe[READ],   STDIN_FILENO);
        dup2(outPipe[WRITE], STDOUT_FILENO);
        dup2(errPipe[WRITE], STDERR_FILENO);

        execvp(m_arguments[0], m_arguments.data());

        // We will only exit here if execvp fails.
        std::exit(FAIL_CODE);
    }

    close(inPipe[READ]);
    close(outPipe[WRITE]);
    close(errPipe[WRITE]);

    m_in  = std::make_unique<Socket>(inPipe[WRITE]);
    m_out = std::make_unique<Socket>(outPipe[READ]);
    m_err = std::make_unique<Socket>(errPipe[READ]);
}

int Process::stop() {
    if (isStopped()) {
        return 0;
    }

    // Killing something that is already dead is harmless.
    kill(m_pid, SIGINT);
    
    int status;
    waitpid(m_pid, &status, 0);
    if (WIFEXITED(status) != 0) {
        status = WEXITSTATUS(status);
    }
    else {
        status = 0;
    }

    m_in.reset();
    m_out.reset();
    m_err.reset();
    m_pid = -1;

    if (status == FAIL_CODE) {
        throw std::invalid_argument(std::format("Failed to execute command: {}", m_command));
    }

    return status;
}

bool Process::in(std::string_view buffer, int waitFor) const {
    if (isStopped()) {
        throw std::runtime_error("Process is not running");
    }

    return m_in->write(std::span(buffer.data(), buffer.size()), waitFor);
}

std::string Process::out(int waitFor) const {
    if (isStopped()) {
        throw std::runtime_error("Process is not running");
    }

    auto buffer = m_out->read(waitFor);
    return std::string(buffer.begin(), buffer.end());
}

std::string Process::err(int waitFor) const {
    if (isStopped()) {
        throw std::runtime_error("Process is not running");
    }

    auto buffer = m_err->read(waitFor);
    return std::string(buffer.begin(), buffer.end());
}

