#include <iostream>
#include <fstream>

#include <boost/json/src.hpp>

#include "utils/File.hpp"
#include "utils/Logger.hpp"

using namespace boost::json;

int main() {
    std::string json = readFile("config.json");
    value val = parse(json);
    object obj = val.as_object();

    logger.info() << value_to<std::string>(obj.at("RGB").at("type"));
    return 0;
}
