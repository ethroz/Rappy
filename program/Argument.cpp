#include <cassert>
#include <charconv>
#include <stdexcept>
#include <vector>
#include <fmt/format.h>

#include "light/Color.hpp"
#include "utils/Duration.hpp"

#include "Argument.hpp"

namespace program {

Argument::Argument(
    std::string_view tag,
    std::string_view explanation,
    bool required,
    int position,
    std::string_view format,
    ParseFunc&& func)
    :
    m_parseFunc(std::move(func)),
    m_tag(tag),
    m_explanation(explanation),
    m_format(format),
    m_position(position),
    m_required(required)
{
    if (tag.empty()) {
        throw std::invalid_argument("Argument cannot have an empty tag");
    }
    if (!m_parseFunc) {
        throw std::logic_error("Must provide a parse function");
    }
}

bool Argument::get(ArgPtr& argPtr, const ArgPtr end) const {
    if (m_position == NPOS) {
        assert(argPtr != end);
        std::string_view arg = *argPtr;
        if (arg.size() <= 2) {
            return false;
        }
        arg.remove_prefix(2);

        if (arg != m_tag) {
            return false;
        }
        argPtr++;
    }

    return m_parseFunc(argPtr, end);
}

std::string Argument::format() const {
    const auto suffix = m_format.empty() ? std::string() : fmt::format(" {}", m_format);
    if (m_position != NPOS) {
        return fmt::format("<{}{}>", m_tag, suffix);
    }
    else if (m_required) {
        return fmt::format("<--{}{}>", m_tag, suffix);
    }
    else {
        return fmt::format("[--{}{}]", m_tag, suffix);
    }
}

// Default specialized constructors. 

// bool
template <>
Argument::Argument(bool& ref, std::string_view tag, std::string_view explanation, bool required, int position) :
    Argument::Argument(tag, explanation, required, position, "",
        [&ref](ArgPtr& argPtr, const ArgPtr end) {
            ref = true;
            return true;
        })
{}

// int
template <>
Argument::Argument(int& ref, std::string_view tag, std::string_view explanation, bool required, int position) :
    Argument::Argument(tag, explanation, required, position, "#",
        [&ref](ArgPtr& argPtr, const ArgPtr end) {
            if (argPtr == end) {
                return false;
            }
            ref = std::stoi(argPtr->data());
            argPtr++;
            return true;
        })
{}

// float
template <>
Argument::Argument(float& ref, std::string_view tag, std::string_view explanation, bool required, int position) :
    Argument::Argument(tag, explanation, required, position, "#.#",
        [&ref](ArgPtr& argPtr, const ArgPtr end) {
            if (argPtr == end) {
                return false;
            }
            ref = std::stof(argPtr->data());
            argPtr++;
            return true;
        })
{}

// string
template <>
Argument::Argument(std::string& ref, std::string_view tag, std::string_view explanation, bool required, int position) :
    Argument::Argument(tag, explanation, required, position, "\"\"",
        [&ref](ArgPtr& argPtr, const ArgPtr end) {
            if (argPtr == end) {
                return false;
            }
            ref = std::string(*argPtr);
            argPtr++;
            return true;
        })
{}

// Duration
template <>
Argument::Argument(Duration& ref, std::string_view tag, std::string_view explanation, bool required, int position) :
    Argument::Argument(tag, explanation, required, position, "#s",
        [&ref](ArgPtr& argPtr, const ArgPtr end) {
            if (argPtr == end) {
                return false;
            }
            ref = Duration(*argPtr);
            argPtr++;
            return true;
        })
{}

// Color
template <>
Argument::Argument(light::Color& ref, std::string_view tag, std::string_view explanation, bool required, int position) :
    Argument::Argument(tag, explanation, required, position, "RGB",
        [&ref](ArgPtr& argPtr, const ArgPtr end) {
            if (argPtr == end) {
                return false;
            }
            ref = light::Color::fromString(*argPtr);
            argPtr++;
            return true;
        })
{}

// int list
template <>
Argument::Argument(std::vector<int>& ref, std::string_view tag, std::string_view explanation, bool required, int position) :
    Argument::Argument(tag, explanation, required, position, "...",
        [&ref](ArgPtr& argPtr, const ArgPtr end) {
            return intListFromArgs(argPtr, end, ref);
        })
{}

bool intListFromArgs(ArgPtr& argPtr, const ArgPtr end, std::vector<int>& list) {
    int i = 0;
    for (; argPtr != end; argPtr++, i++) {
        int num;
        auto result = std::from_chars(argPtr->data(), argPtr->data() + argPtr->size(), num);
        if (result.ec != std::errc()) {
            break;
        }
        list.push_back(num);
    }

    return i > 0;
}

} // namespace program

