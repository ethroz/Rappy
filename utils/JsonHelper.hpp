#pragma once

#include <exception>
#include <format>
#include <vector>

#include <boost/json.hpp>


#include "utils/Duration.hpp"

#include <cxxabi.h>

boost::json::value parse(std::string_view json);

template<typename T>
T getAsOr(const boost::json::value& val, const T& dflt) {
    try {
        return boost::json::value_to<const T&>(val);
    }
    catch (const std::exception& e) {
        return dflt;
    }
}

template<typename T>
T getAsOrThrow(const boost::json::value& val, std::string_view prefix) {
    try {
        return boost::json::value_to<T>(val);
    }
    catch (const std::exception& e) {
        throw std::invalid_argument(std::format("{}: expected a {}", prefix, typeid(T).name()));
    }
}

template<typename T>
T getAsOr(const boost::json::object& obj, std::string_view key, const T& dflt) {
    if (auto* v = obj.if_contains(key)) {
        return boost::json::value_to<T>(*v);
    }
    else {
        return dflt;
    }
}

template<typename T>
T getAsOrThrow(const boost::json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        return boost::json::value_to<T>(*v);
    }
    else {
        throw std::invalid_argument(std::format("{}: Missing \"{}\" key", prefix, key));
    }
}

template<typename T>
std::vector<T> getAsVecOr(const boost::json::object& obj, std::string_view key, const std::vector<T>& dflt) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_array()) {
            return boost::json::value_to<std::vector<T>>(*v);
        }
    }
    return dflt;
}

template<typename T>
std::vector<T> getAsVecOrThrow(const boost::json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_array()) {
            return boost::json::value_to<std::vector<T>>(*v);
        }
        else {
            throw std::invalid_argument(std::format("{}: \"{}\" is not an array", prefix, key));
        }
    }
    else {
        throw std::invalid_argument(std::format("{}: Missing \"{}\" key", prefix, key));
    }
}

Duration getAsDurationOr(const boost::json::object& obj, std::string_view key, Duration dflt);
Duration getAsDurationOrThrow(const boost::json::object& obj, std::string_view key, std::string_view prefix);
const boost::json::object& getAsObjectOrThrow(const boost::json::value& val, std::string_view prefix);
const boost::json::array& getAsArrayOrThrow(const boost::json::value& val, std::string_view prefix);
