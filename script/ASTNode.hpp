#pragma once

#include <format>
#include <functional>
#include <memory>
#include <regex>
#include <string>
#include <string_view>


#include "utils/Other.hpp"

namespace script {

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::function<float()> get() const = 0;
};

class NumberNode : public ASTNode {
public:
    NumberNode(std::string_view arg) : m_value(to<float>(arg)) {}

    static bool match(std::string_view str) {
        return std::regex_match(str.begin(), str.begin() + 1, std::regex("[+-.0-9]"));
    }

    std::function<float()> get() const override {
        return [value = m_value]() { return value; };
    }

private:
    const float m_value;
};

class ArgumentNode : public ASTNode {
public:
    ArgumentNode(std::string_view arg) : m_name(arg), m_ptr(std::make_shared<float>(0.0f)) {}

    static bool match(std::string_view str) {
        return std::regex_match(str.begin(), str.begin() + 1, std::regex("[a-zA-Z]"));
    }

    std::function<float()> get() const override {
        return [arg = m_ptr]() { return *arg; };
    }

    std::string_view name() const { return m_name; }
    std::shared_ptr<float> ptr() const { return m_ptr; }

private:
    const std::string m_name;
    std::shared_ptr<float> m_ptr;
};

class BinaryOpNode : public ASTNode {
public:
    BinaryOpNode(char op, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right)
        : m_op(op), m_left(std::move(left)), m_right(std::move(right))
    {}

    static bool match(std::string_view str) {
        return std::regex_match(str.begin(), str.begin() + 1, std::regex(R"([+\-*/^])"));
    }

    static int priority(char op) {
        switch (op) {
        case '+': return 1;
        case '-': return 1;
        case '*': return 2;
        case '/': return 2;
        case '^': return 3;
        default: throw std::invalid_argument(std::format("Invalid operator: {}", op));
        }
    }

    std::function<float()> get() const override {
        std::function<float()> left = m_left->get();
        std::function<float()> right = m_right->get();
        switch (m_op) {
        case '+': return [left = std::move(left), right = std::move(right)]() { return left() + right(); }; break;
        case '-': return [left = std::move(left), right = std::move(right)]() { return left() - right(); }; break;
        case '*': return [left = std::move(left), right = std::move(right)]() { return left() * right(); }; break;
        case '/': return [left = std::move(left), right = std::move(right)]() { return left() / right(); }; break;
        case '^': return [left = std::move(left), right = std::move(right)]() { return std::pow(left(), right()); }; break;
        default: throw std::invalid_argument(std::format("Invalid operator: {}", m_op));
        }
    }

private:
    const char m_op;
    const std::shared_ptr<ASTNode> m_left, m_right;
};

} // namespace script
