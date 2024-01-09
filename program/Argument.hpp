#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace program {

using ArgPtr = std::vector<std::string_view>::iterator;
using ParseFunc = std::function<bool(ArgPtr& argPtr, const ArgPtr end)>;

class Argument {
public:
    template <typename T>
    Argument(
        T& ref,
        std::string_view tag,
        std::string_view explanation,
        bool required,
        int position = NPOS);

    Argument(
        std::string_view tag,
        std::string_view explanation,
        bool required,
        int position,
        std::string_view format,
        ParseFunc&& func);

    bool get(ArgPtr& argPtr, const ArgPtr end) const;
    
    std::string format() const;

    const std::string& tag() const { return m_tag; }
    const std::string& explanation() const { return m_explanation; }
    bool required() const { return m_required; }
    int position() const { return m_position; }

    static constexpr int NPOS = -1;

private:
    const ParseFunc m_parseFunc;
    const std::string m_tag;
    const std::string m_explanation;
    const std::string m_format;
    const int m_position;
    const bool m_required;
};

} // namespace program

