#pragma once

#include <vector>

#include <boost/json.hpp>

#include <fmt/format.h>

#include "utils/Duration.hpp"

using namespace boost;

json::value parse(std::string_view json);

template<typename T>
T getAsOr(const json::object& obj, std::string_view key, T dflt) {
    if (auto* v = obj.if_contains(key)) {
        return json::value_to<T>(*v);
    }
    else {
        return dflt;
    }
}

template<typename T>
T getAsOrThrow(const json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        return json::value_to<T>(*v);
    }
    else {
        throw std::invalid_argument(fmt::format("{}: Missing \"{}\" key", prefix, key));
    }
}

template<typename T>
std::vector<T> getAsVecOr(const json::object& obj, std::string_view key, T dflt) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_array()) {
            return json::value_to<std::vector<T>>(*v);
        }
    }
    return dflt;
}

template<typename T>
std::vector<T> getAsVecOrThrow(const json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_array()) {
            return json::value_to<std::vector<T>>(*v);
        }
        else {
            throw std::invalid_argument(fmt::format("{}: \"{}\" is not an array", prefix, key));
        }
    }
    else {
        throw std::invalid_argument(fmt::format("{}: Missing \"{}\" key", prefix, key));
    }
}

std::string_view getAsStringViewOr(const json::object& obj, std::string_view key, std::string_view dflt);
std::string_view getAsStringViewOrThrow(const json::object& obj, std::string_view key, std::string_view prefix);
float getAsFloatOr(const json::object& obj, std::string_view key, float dflt);
float getAsFloatOrThrow(const json::object& obj, std::string_view key, std::string_view prefix);
int getAsIntOr(const json::object& obj, std::string_view key, int dflt);
int getAsIntOrThrow(const json::object& obj, std::string_view key, std::string_view prefix);
bool getAsBoolOr(const json::object& obj, std::string_view key, bool dflt);
bool getAsBoolOrThrow(const json::object& obj, std::string_view key, std::string_view prefix);
Duration getAsDurationOr(const json::object& obj, std::string_view key, Duration dflt);
Duration getAsDurationOrThrow(const json::object& obj, std::string_view key, std::string_view prefix);
const json::object& getAsObjectOrThrow(const json::value& val, std::string_view prefix);
const json::array& getAsArrayOrThrow(const json::value& val, std::string_view prefix);
