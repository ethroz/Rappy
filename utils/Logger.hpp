#pragma once

#include <iostream>

#include "Other.hpp"

class Log {
public:
    explicit Log(std::ostream& stream = std::cout, bool flush = true) : m_out(stream), m_flush(flush) {}
    Log(const Log& other) : m_out(other.m_out), m_flush(true) {}
    ~Log() { if (m_flush) m_out << std::endl; }

    template <typename T>
    const Log& operator<<(const T& val) const { m_out << val; return *this; }

private:
    std::ostream& m_out;
    bool m_flush;
};

class Logger {
public:
    Logger(std::ostream& stream = std::cout) : m_out(stream) {}

    template <typename T>
    Log operator<<(const T& val) const { return Log(m_out, false) << val; }

private:
    std::ostream& m_out;
};

