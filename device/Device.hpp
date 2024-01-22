#pragma once

#include <string_view>

#include "utils/Socket.hpp"

namespace device {

template <typename State>
class Device {
public:
    explicit Device(std::string_view id);

    const State& read();

private:
    const Socket m_socket;
    State m_state;
};

} // namespace device

