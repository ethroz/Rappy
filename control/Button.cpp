#include "Button.hpp"

namespace control {

Button::Button(int pin, bool toggle) :
    pi::Input("Button"),
    m_pin(wiring::Pin::create({pin, wiring::PinMode::IN, true})),
    m_toggle(toggle)
{}

void Button::poll() {
    const auto pressed = m_pin->get() == 1.0f;

    // Rising edge.
    if (pressed && !m_last && (!m_toggle || !m_value)) {
        m_value = true;
    }
    // Falling edge.
    else if ((!m_toggle && !pressed && m_last) || (pressed && !m_last && m_value)) {
        m_value = false;
    }

    m_last = pressed;
}

pi::Producer Button::getProducer(std::string_view key) const {
    if (key != "value") {
        throw std::invalid_argument("control::Button::read(): only accepts \"value\"");
    }
    return [this]() { return m_value ? 1.0f : 0.0f; };
}

} // namespace control
