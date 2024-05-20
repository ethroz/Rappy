#include <boost/json/src.hpp>

#include "JsonHelper.hpp"

json::value parse(std::string_view json) {
    return json::parse(json);
}

std::string_view getAsStringViewOr(const json::object& obj, std::string_view key, std::string_view dflt) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_string()) {
            return json::value_to<std::string_view>(*v);
        }
    }
    return dflt;
}

std::string_view getAsStringViewOrThrow(const json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_string()) {
            return json::value_to<std::string_view>(*v);
        }
        else {
            throw std::invalid_argument(fmt::format("{}: \"{}\" is not a string", prefix, key));
        }
    }
    else {
        throw std::invalid_argument(fmt::format("{}: Missing \"{}\" key", prefix, key));
    }
}

float getAsFloatOr(const json::object& obj, std::string_view key, float dflt) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_number()) {
            return json::value_to<float>(*v);
        }
    }
    return dflt;
}

float getAsFloatOrThrow(const json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_number()) {
            return json::value_to<float>(*v);
        }
        else {
            throw std::invalid_argument(fmt::format("{}: \"{}\" is not a float", prefix, key));
        }
    }
    else {
        throw std::invalid_argument(fmt::format("{}: Missing \"{}\" key", prefix, key));
    }
}

int getAsIntOr(const json::object& obj, std::string_view key, int dflt) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_int64() || v->is_uint64()) {
            return json::value_to<int>(*v);
        }
    }
    return dflt;
}

int getAsIntOrThrow(const json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_int64() || v->is_uint64()) {
            return json::value_to<int>(*v);
        }
        else {
            throw std::invalid_argument(fmt::format("{}: \"{}\" is not an int", prefix, key));
        }
    }
    else {
        throw std::invalid_argument(fmt::format("{}: Missing \"{}\" key", prefix, key));
    }
}

bool getAsBoolOr(const json::object& obj, std::string_view key, bool dflt) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_bool()) {
            return json::value_to<bool>(*v);
        }
    }
    return dflt;
}

bool getAsBoolOrThrow(const json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_bool()) {
            return json::value_to<bool>(*v);
        }
        else {
            throw std::invalid_argument(fmt::format("{}: \"{}\" is not a boolean", prefix, key));
        }
    }
    else {
        throw std::invalid_argument(fmt::format("{}: Missing \"{}\" key", prefix, key));
    }
}

Duration getAsDurationOr(const json::object& obj, std::string_view key, Duration dflt) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_number()) {
            return Duration(json::value_to<float>(*v));
        }
        else if (v->is_string()) {
            return Duration(json::value_to<std::string_view>(*v));
        }
    }
    return dflt;
}

Duration getAsDurationOrThrow(const json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_number()) {
            return Duration(json::value_to<float>(*v));
        }
        else if (v->is_string()) {
            return Duration(json::value_to<std::string_view>(*v));
        }
        else {
            throw std::invalid_argument(fmt::format("{}: \"{}\" is not a float", prefix, key));
        }
    }
    else {
        throw std::invalid_argument(fmt::format("{}: Missing \"{}\" key", prefix, key));
    }
}

const json::object& getAsObjectOrThrow(const json::value& val, std::string_view prefix) {
    if (val.is_object()) {
        return val.as_object();
    }
    else {
        throw std::invalid_argument(fmt::format("{}: Expected object, received: {}", prefix, json::value_to<std::string_view>(val)));
    }
}

const json::array& getAsArrayOrThrow(const json::value& val, std::string_view prefix) {
    if (val.is_array()) {
        return val.as_array();
    }
    else {
        throw std::invalid_argument(fmt::format("{}: Expected array, received: {}", prefix, json::value_to<std::string_view>(val)));
    }
}
