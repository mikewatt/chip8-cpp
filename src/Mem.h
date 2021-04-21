#pragma once

#include <cstdint>
#include <vector>

class Mem {
    public:
        Mem();

        bool init_from_file(const char *filename);
        uint8_t read(uint16_t addr) const;
        void write(uint16_t addr, uint8_t value);

    private:
        std::vector<uint8_t> m_memory;
        std::vector<uint8_t> m_rom;
};
