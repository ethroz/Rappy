#pragma once

namespace terminal {

void clear(bool flush = true);

void deleteLine(bool flush = true);

void gotoxy(uint32_t x, uint32_t y, bool flush = true);

void up(uint32_t n, bool flush = true);

void down(uint32_t n, bool flush = true);

void left(uint32_t n, bool flush = true);
    
void right(uint32_t n, bool flush = true);
 
} // namespace terminal

