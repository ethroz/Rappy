#pragma once

#include "utils/Socket.hpp"

#include "ControllerState.hpp"
#include "pi/Input.hpp"

namespace device {

class Controller : public pi::Input {
public:
    explicit Controller(std::string_view id);
    virtual ~Controller() override {}

    void poll() override;

    pi::Producer getProducer(std::string_view key) const override;

private:
    const Socket m_socket;
    ControllerState m_state;
};

} // namespace device
