#pragma once

#include <memory>

#include "wiring/Pin.hpp"

#include "pi/Input.hpp"

namespace control {

class Button : public pi::Input {
public:
    Button(int pin, bool toggle);
    virtual ~Button() override {}

    void poll();

    pi::Producer getProducer(std::string_view key) const override;

private:
    std::unique_ptr<wiring::Pin> m_pin;
    const bool m_toggle;
    bool m_last = false;
    bool m_value = false;
};

} // namespace control
