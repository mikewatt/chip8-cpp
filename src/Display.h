#pragma once

#include <array>
#include <cstdint>

class Display {
    public:
        static const uint8_t WIDTH = 64;
        static const uint8_t HEIGHT = 32;

        void clear();
        void draw_xor(uint8_t x, uint8_t y);
        bool is_on(uint8_t x, uint8_t y);

    private:
        std::array<bool, WIDTH * HEIGHT> m_vram;
};
