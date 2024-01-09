#pragma once

#include <chrono>

class Duration {
public:
    Duration() = default;
    Duration(std::string_view str) { set(str); }
    Duration(float value) { set(value); }

    template <class Rep, class Period>
    Duration(std::chrono::duration<Rep, Period> value) { set(value); }

    void set(std::string_view str); 
    void set(float value) { m_value = value; }

    float get() const { return m_value; }
    
    template <class Rep, class Period>
    void set(std::chrono::duration<Rep, Period> value) {
        using namespace std::chrono;
        m_value = duration_cast<duration<float>>(value).count();
    }

    std::chrono::nanoseconds ns() const;

    operator float() const { return get(); }

private:
    float m_value = 0.0f;
};

