#include <string>
#include <vector>

#include "script/Parser.hpp"
#include "utils/Logger.hpp"
#include "utils/Timer.hpp"

constexpr int MAX = 1000000;
constexpr int SIZE = 8;

int main(int argc, char* argv[]) {
    float data[SIZE];

    if (argc != 3) {
        logger.error() << "Expected two arguments";
        return 1;
    }
    float arg0 = std::stof(std::string(argv[1]));
    float arg1 = std::stof(std::string(argv[2]));

    auto func = script::parse<float,float>("(a,b){a+b}");
    Timer timer;

    logger.info() << "Running script function";
    timer.start();
    for (int i = 0; i < MAX; i++) {
        data[i % SIZE] = func(arg0, arg1);
    }
    timer.stop();
    logger.info() << "Script function ran in: " << timer.elapsed().get() << " s";

    logger.info() << "Running raw function";
    timer.start();
    for (int i = 0; i < MAX; i++) {
        data[i % SIZE] = arg0 + arg1;
    }
    timer.stop();
    logger.info() << "Raw function ran in: " << timer.elapsed().get() << " s";

    return 0;
}
