#pragma once

#include <string_view>

#include "utils/Socket.hpp"

namespace bluetooth {

class Device {
public:
    static Device fromAddress(const std::string& address);
    static Device fromName(std::string_view name, int timeout);

private:
    explicit Device(Socket&& socket) : m_socket(std::move(socket)) {}

    const Socket m_socket;
};

} // namespace bluetooth

