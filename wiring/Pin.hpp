#pragma once

#include <cstdint>
#include <memory>

#include "Context.hpp"
#include "PinConfig.hpp"

namespace wiring {

class Pin {
public:
    virtual ~Pin() {}

    static std::unique_ptr<Pin> create(const PinConfig& config);

    virtual void set(float val) = 0;
    virtual float get() = 0;

protected:
    Pin(int pin);

    const int m_pin;

private:
    const Context m_ctx;
};

class OutputPin : public Pin {
public:
    OutputPin(const PinConfig& config);
    virtual ~OutputPin() override { set(0.0f); }

    virtual void set(float val) override;
    virtual float get() override { return m_val; }

protected:
    OutputPin(int pin, bool invert) : Pin(pin), m_invert(invert) {}

    const bool m_invert;
    float m_val = 0.0f;
};

class PwmPin : public OutputPin {
public:
    PwmPin(const PinConfig& config);
    ~PwmPin() override { set(0.0f); }

    void set(float val) override;
};

class ServoPin : public OutputPin {
public:
    ServoPin(const PinConfig& config);
    ~ServoPin() override { set(0.0f); }
    
    //  1.0 -> 2500 us
    //  0.0 -> 1500 us
    // -1.0 ->  500 us
    void set(float val) override;
};

class InputPin : public Pin {
public:
    InputPin(const PinConfig& config);

    virtual void set(float val) override;
    virtual float get() override;

protected:
    InputPin(int pin, bool invert) : Pin(pin), m_invert(invert) {}

    const bool m_invert;
};

} // namespace wiring

