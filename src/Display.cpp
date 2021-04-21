#include "Display.h"
#include <cstdio>

void Display::clear() {
    m_vram.fill(false);
}

bool Display::is_on(uint8_t x, uint8_t y)  {
    auto scr_y = y % HEIGHT;
    auto scr_x = x % WIDTH;
    return m_vram[y * WIDTH + x];
}

void Display::draw_xor(uint8_t x, uint8_t y) {
    auto scr_y = y % HEIGHT;
    auto scr_x = x % WIDTH;
    m_vram[scr_y * WIDTH + scr_x] ^= true;
}