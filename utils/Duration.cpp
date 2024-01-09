#include <stdexcept>
#include <string>
#include <fmt/core.h>

#include "Duration.hpp"

void Duration::set(std::string_view str) {
    if (str.empty()) {
        throw std::invalid_argument("Unrecognized time: Empty string");
    }

    constexpr float THOUSAND = 1000.0f;
    float scale = 1.0f;
    
    if (str.ends_with('s')) {
        str.remove_suffix(1);
    }

    if (!str.empty()) {
        switch (str.back()) {
        case 'n':
            scale /= THOUSAND;
            [[fallthrough]];
        case 'u':
            scale /= THOUSAND;
            [[fallthrough]];
        case 'm':
            scale /= THOUSAND;
            str.remove_suffix(1);
        default:
            break;
        }
    }
    else {
        throw std::invalid_argument(fmt::format("Unrecognized time: {}", str));
    }

    const auto time = std::stof(std::string(str));
    m_value = time * scale;
}

std::chrono::nanoseconds Duration::ns() const {
    using namespace std::chrono;
    return round<nanoseconds>(duration<float>{m_value});
}

