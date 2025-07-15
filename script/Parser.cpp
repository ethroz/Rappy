#include <cassert>
#include <format>
#include <regex>
#include <span>

#include "utils/Logger.hpp"

#include "ASTNode.hpp"

#include "Parser.hpp"

namespace script {

using svmatch = std::match_results<std::string_view::const_iterator>;
using svregex_iterator = std::regex_iterator<std::string_view::const_iterator>;

std::shared_ptr<ASTNode> parseNumber(std::string_view& expr) {
    // TODO: replace this with std::from_chars once clang is working.
    std::regex numberRegex{ R"(([+-]?([0-9]+([.][0-9]*)?([eE][+-]?[0-9]+)?|[.][0-9]+([eE][+-]?[0-9]+)?)).*)" };
    svmatch matcher;
    if (!std::regex_match(expr.begin(), expr.end(), matcher, numberRegex)) {
        throw std::invalid_argument(std::format("Parse error: Unexpected number: {}", expr));
    }
    assert(matcher.size() >= 2);

    auto number = std::string_view(matcher[1].first, matcher[1].second);
    expr.remove_prefix(number.size());
    return std::make_shared<NumberNode>(number);
}

std::shared_ptr<ASTNode> parseArgument(std::string_view& expr, std::span<std::shared_ptr<ArgumentNode>> args) {
    for (const auto& arg : args) {
        if (expr.starts_with(arg->name())) {
            expr.remove_prefix(arg->name().size());
            return arg;
        }
    }
    throw std::invalid_argument(std::format("Parse error: Unexpected argument: {}", expr));
}

std::shared_ptr<ASTNode> parseExpression(std::string_view& expr, std::span<std::shared_ptr<ArgumentNode>> args) {
    std::vector<std::shared_ptr<ASTNode>> nodes;
    std::vector<char> ops;

    while (true) {
        if (expr.empty()) {
            throw std::invalid_argument("Parse error: Unexpected end of script");
        }
        else if (expr[0] == '(') {
            expr.remove_prefix(1);
            nodes.push_back(parseExpression(expr, args));
        }
        else if (expr[0] == ')') {
            expr.remove_prefix(1);
            break;
        }
        else if (ArgumentNode::match(expr)) {
            nodes.push_back(parseArgument(expr, args));
        }
        else if (BinaryOpNode::match(expr) && nodes.size() > ops.size()) {
            ops.push_back(expr[0]);
            expr.remove_prefix(1);
        }
        else if (NumberNode::match(expr)) {
            nodes.push_back(parseNumber(expr));
        }
        else {
            throw std::invalid_argument(std::format("Parse error: Unexpected token: {}", expr));
        }
    }

    if (nodes.empty()) {
        throw std::invalid_argument("Parse error: Cannot have empty brackets");
    }

    if (nodes.size() != ops.size() + 1) {
        throw std::invalid_argument("Parse error: Must provide an argument on either side of an operation");
    }

    while (!ops.empty()) {
        assert(nodes.size() == ops.size() + 1);
        int largest = 0;
        int index = -1;
        for (size_t i = 0; i < ops.size(); i++) {
            int priority = BinaryOpNode::priority(ops[i]);
            if (priority > largest) {
                largest = priority;
                index = i;
            }
        }
        assert(index >= 0);
        nodes[index] = std::make_shared<BinaryOpNode>(ops[index], std::move(nodes[index]), std::move(nodes[index + 1]));
        nodes.erase(nodes.begin() + index + 1);
        ops.erase(ops.begin() + index);
    }

    return nodes[0];
}

std::vector<std::shared_ptr<ArgumentNode>> parseArguments(std::string_view args, size_t expected) {
    std::vector<std::shared_ptr<ArgumentNode>> arguments;

    std::regex argRegex{ R"([a-zA-Z][_a-zA-Z0-9]*)" };
    const auto argsEnd = svregex_iterator();
    for (auto arg = svregex_iterator(args.begin(), args.end(), argRegex); arg != argsEnd; ++arg) {
        const auto& subMatch = (*arg)[0];
        arguments.emplace_back(std::make_shared<ArgumentNode>(std::string_view(subMatch.first, subMatch.second)));
    }

    if (arguments.size() != expected) {
        throw std::invalid_argument(std::format("Expected function to have {} argument{}", expected, plural(expected)));
    }

    return arguments;
}

std::pair<std::string_view, std::string_view> parseFunction(std::string& script) {
    script.erase(std::remove_if(script.begin(), script.end(), [](char c) { return std::isspace(c); }), script.end());

    std::regex funcRegex{ R"(\((.*)\)\{(.+\}))" };
    std::smatch matcher;
    if (!std::regex_match(script, matcher, funcRegex) || matcher.size() < 3) {
        throw std::invalid_argument(std::format("Script did not look like a function: \"{}\"", script));
    }

    auto args = std::string_view(matcher[1].first, matcher[1].second);
    auto expr = std::string_view(matcher[2].first, matcher[2].second);
    script.back() = ')';
    return { args, expr };
}

template<>
std::function<float()> parse(std::string script) {
    auto [args, expr] = parseFunction(script);
    auto arguments = parseArguments(args, 0);
    auto node = parseExpression(expr, arguments);
    return node->get();
}

template<>
std::function<float(float)> parse(std::string script) {
    auto [args, expr] = parseFunction(script);
    auto arguments = parseArguments(args, 1);
    auto node = parseExpression(expr, arguments);
    return [
        arg0 = arguments[0]->ptr(),
        func = node->get()
    ]
    (float x) {
        *arg0 = x;
        return func();
    };
}

template<>
std::function<float(float, float)> parse(std::string script) {
    auto [args, expr] = parseFunction(script);
    auto arguments = parseArguments(args, 2);
    auto node = parseExpression(expr, arguments);
    return [
        arg0 = arguments[0]->ptr(),
        arg1 = arguments[1]->ptr(),
        func = node->get()
    ]
    (float x, float y) {
        *arg0 = x;
        *arg1 = y;
        return func();
    };
}

template<>
std::function<float(float, float, float)> parse(std::string script) {
    auto [args, expr] = parseFunction(script);
    auto arguments = parseArguments(args, 3);
    auto node = parseExpression(expr, arguments);
    return [
        arg0 = arguments[0]->ptr(),
        arg1 = arguments[1]->ptr(),
        arg2 = arguments[2]->ptr(),
        func = node->get()
    ]
    (float x, float y, float z) {
        *arg0 = x;
        *arg1 = y;
        *arg2 = z;
        return func();
    };
}

} // namespace script
