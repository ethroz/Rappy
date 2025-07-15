#include <format>
#include <iostream>

#include "Terminal.hpp"

namespace terminal {

void clear(bool flush) {
    std::cout << "\033[2J";
    gotoxy(0,0,false);
    if (flush) {
        std::cout.flush();
    }
}

void deleteLine(bool flush) {
    std::cout << "\033[2K\r";
    if (flush) {
        std::cout.flush();
    }
}

void gotoxy(uint32_t x, uint32_t y, bool flush) {
    std::cout << std::format("\033[{};{}H", y + 1, x + 1);
    if (flush) {
        std::cout.flush();
    }
}

void up(uint32_t n, bool flush) {
    std::cout << std::format("\033[{}A", n);
    if (flush) {
        std::cout.flush();
    }
}

void down(uint32_t n, bool flush) {
    std::cout << std::format("\033[{}B", n);
    if (flush) {
        std::cout.flush();
    }
}

void left(uint32_t n, bool flush) {
    std::cout << std::format("\033[{}D", n);
    if (flush) {
        std::cout.flush();
    }
}

void right(uint32_t n, bool flush) {
    std::cout << std::format("\033[{}C", n);
    if (flush) {
        std::cout.flush();
    }
}

} // namespace terminal

