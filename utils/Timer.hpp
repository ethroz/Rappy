#pragma once

#include <chrono>

class Timer {
public:
    void start();
    void reset();
    void stop();
    float elapsed() const;
    bool running() const { return m_running; }

private:
    using Clock = std::chrono::steady_clock;
    using TS = std::chrono::time_point<Clock>;

    TS m_timeStart;
    TS m_timeStop;
    bool m_running = false;
};

