#include <fstream>
#include <iostream>
#include <sstream>
#include <fmt/format.h>

#include "File.hpp"

std::string readFile(const std::string& path) {
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        throw std::invalid_argument(fmt::format("Could not open the file: '{}'", path));
    }
    
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    return buffer.str();
}
