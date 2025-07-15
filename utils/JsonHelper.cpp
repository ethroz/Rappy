#include <boost/json/src.hpp>

#include "JsonHelper.hpp"

boost::json::value parse(std::string_view json) {
    return boost::json::parse(json);
}

Duration getAsDurationOr(const boost::json::object& obj, std::string_view key, Duration dflt) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_number()) {
            return Duration(boost::json::value_to<float>(*v));
        }
        else if (v->is_string()) {
            return Duration(boost::json::value_to<std::string_view>(*v));
        }
    }
    return dflt;
}

Duration getAsDurationOrThrow(const boost::json::object& obj, std::string_view key, std::string_view prefix) {
    if (auto* v = obj.if_contains(key)) {
        if (v->is_number()) {
            return Duration(boost::json::value_to<float>(*v));
        }
        else if (v->is_string()) {
            return Duration(boost::json::value_to<std::string_view>(*v));
        }
        else {
            throw std::invalid_argument(std::format("{}: \"{}\" is not a duration", prefix, key));
        }
    }
    else {
        throw std::invalid_argument(std::format("{}: Missing \"{}\" key", prefix, key));
    }
}

const boost::json::object& getAsObjectOrThrow(const boost::json::value& val, std::string_view prefix) {
    if (val.is_object()) {
        return val.as_object();
    }
    else {
        throw std::invalid_argument(std::format("{}: Expected object, received: {}", prefix, boost::json::value_to<std::string_view>(val)));
    }
}

const boost::json::array& getAsArrayOrThrow(const boost::json::value& val, std::string_view prefix) {
    if (val.is_array()) {
        return val.as_array();
    }
    else {
        throw std::invalid_argument(std::format("{}: Expected array, received: {}", prefix, boost::json::value_to<std::string_view>(val)));
    }
}
