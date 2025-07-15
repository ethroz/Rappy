#pragma once

#include <functional>
#include <map>
#include <string_view>

#include <boost/json.hpp>

#include "Input.hpp"
#include "Output.hpp"

namespace pi {

using Connection = std::function<void()>;

Connection parseConnection(
    const boost::json::object& cfg,
    const std::map<std::string_view, std::unique_ptr<Input>>& inputs,
    const std::map<std::string_view, std::unique_ptr<Output>>& outputs);

} // namespace pi
