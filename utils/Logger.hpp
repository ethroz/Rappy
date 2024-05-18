#pragma once

#include <iostream>

#include "Other.hpp"

class LogObject {
public:
    ~LogObject() { if (m_endl) m_out << '\n'; if (m_flush) m_out << std::flush; }

    template <typename T>
    const LogObject& operator<<(const T& val) const { m_out << val; return *this; }

private:
    friend class Logger;
    explicit LogObject(std::ostream& stream, bool endl, bool flush) : m_out(stream), m_endl(endl), m_flush(flush) {}

    std::ostream& m_out;
    bool m_endl;
    bool m_flush;
};

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance(std::cout, std::cerr);
        return instance;
    }

    LogObject fatal(bool endl = true, bool flush = true) const { return LogObject(m_err, endl, flush); }
    LogObject error(bool endl = true, bool flush = true) const { return LogObject(m_err, endl, flush); }
    LogObject warning(bool endl = true, bool flush = true) const { return LogObject(m_out, endl, flush); }
    LogObject info(bool endl = true, bool flush = true) const { return LogObject(m_out, endl, flush); }
    LogObject debug(bool endl = true, bool flush = true) const { return LogObject(m_out, endl, flush); }
    LogObject trace(bool endl = true, bool flush = true) const { return LogObject(m_out, endl, flush); }

private:
    Logger(std::ostream& outStream, std::ostream& errStream) : m_out(outStream), m_err(errStream) {}
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ostream& m_out;
    std::ostream& m_err;
};

static const Logger& logger = Logger::getInstance();
