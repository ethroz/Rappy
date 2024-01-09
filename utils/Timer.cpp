#include "Timer.hpp"

using namespace std::chrono_literals;

void Timer::start() { 
    m_running = true; 
    reset();
}

void Timer::reset() { 
    m_timeStart = Clock::now();
}

void Timer::stop() { 
    m_running = false;
    m_timeStop = Clock::now();
}

float Timer::elapsed() const {
    TS timeStop = m_running ? Clock::now() : m_timeStop;
    return std::chrono::duration<float>(timeStop - m_timeStart).count();
}

