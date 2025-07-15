#pragma once

#include <functional>
#include <string_view>

namespace script {

template<class... Args>
std::function<float(Args...)> parse(std::string script);

} // namespace script
