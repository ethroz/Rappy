#include <cerrno>
#include <system_error>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fmt/core.h>

#include "Other.hpp"

#include "Socket.hpp"

enum class IoType : uint8_t {
    IN,
    OUT,
    CLOSE
};

// Local function.
[[nodiscard]]
bool checkFor(int fd, IoType type, int timeout = 0) {
    struct pollfd fdData{};
    fdData.fd = fd;
    switch (type) {
    case IoType::IN:    fdData.events = POLLIN;            break;
    case IoType::OUT:   fdData.events = POLLOUT;           break;
    case IoType::CLOSE: fdData.events = POLLERR | POLLHUP; break;
    default: throw std::invalid_argument(fmt::format("Unknown IO type: {}", to_underlying(type)));
    }

    while (poll(&fdData, 1, timeout) < 0) {
        const auto error = errno;
        if (error != EINTR) {
            throw std::system_error(error, std::generic_category(), "poll");
        }
    }
    
    return (fdData.events & fdData.revents) > 0;
}

Socket::Socket(int domain, int type, int protocol) : m_fd(socket(domain, type, protocol)) {
    if (m_fd < 0) {
        throw std::system_error(errno, std::generic_category(), "socket");
    }
}

Socket::Socket(int fd) : m_fd(fd) {
    if (m_fd < 0) {
        throw std::invalid_argument("Invalid file descriptor");
    }
}

Socket::~Socket() {
    close(m_fd);
}

bool Socket::isClosed() const {
    return checkFor(m_fd, IoType::OUT);
}

bool Socket::write(std::span<const char> buffer, int waitFor) const {
    if (waitFor != BLOCK && !checkFor(m_fd, IoType::OUT, waitFor)) {
        return false;
    }

    const auto bytesWritten = ::write(m_fd, buffer.data(), buffer.size());
    if (bytesWritten < 0) {
        throw std::system_error(errno, std::generic_category(), "write");
    }

    return bytesWritten == static_cast<decltype(bytesWritten)>(buffer.size());
}

std::vector<char> Socket::read(int waitFor) const {
    constexpr auto INC = 1024;
    size_t totalSize = 0;
    std::vector<char> buffer;

    while (checkFor(m_fd, IoType::IN, waitFor)) {
        buffer.resize(totalSize + INC);
        const auto bytesRead = ::read(m_fd, buffer.data() + totalSize, INC);
        if (bytesRead < 0) {
            throw std::system_error(errno, std::generic_category(), "read");
        }

        totalSize += bytesRead;
    }

    buffer.resize(totalSize);
    return buffer;
}

