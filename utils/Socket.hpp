#pragma once

#include <span>
#include <vector>

static constexpr int BLOCK = -1;
static constexpr int NO_BLOCK = 0;

class Socket {
public:
    Socket(int domain, int type, int protocol);
    Socket(int fd);
    ~Socket();

    [[nodiscard]] int fd() const { return m_fd; }

    [[nodiscard]] bool isClosed() const;

    // waitFor is in ms.
    bool write(std::span<const char> buffer, int waitFor = NO_BLOCK) const;
    
    // waitFor is in ms.
    [[nodiscard]] std::vector<char> read(int waitFor = NO_BLOCK) const;

    operator int() const { return fd(); }

private:
    const int m_fd;
};

