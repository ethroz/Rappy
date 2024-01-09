#pragma once

#include <string>

namespace pigpio {

std::string error(int code);

int checkError(int code);

} // namespace pigpio

