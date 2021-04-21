#pragma once

#include "Mem.h"
#include "Display.h"
#include "Keypad.h"
#include <cstdint>
#include <optional>

class Chip8 {
    public:
        Chip8(Mem& memory, Display& display, Keypad& keypad);

        void step();
        void timer_interrupt();
        bool is_speaker_on() { return m_speaker_on; };

    private:
        Mem& m_memory;
        Display& m_display;
        Keypad& m_keypad;

        bool m_speaker_on;
        std::optional<uint8_t> m_key_register;
        
        uint16_t m_pc { 0x200 };
        uint8_t m_registers[16]{};

        uint8_t m_sp{};
        uint16_t m_stack[16]{};

        uint16_t m_reg_i{};
        uint16_t m_reg_st{};
        uint16_t m_reg_dt{};

        // Instruction implementations
        void UNKNOWN(uint16_t);
        void CLS(uint16_t);
        void RET(uint16_t);
        void JP(uint16_t);
        void CALL(uint16_t);
        void SE_REG_IMM(uint16_t);
        void SNE_REG_IMM(uint16_t);
        void SE_REG_REG(uint16_t);
        void LD_REG_IMM(uint16_t);
        void ADD_REG_IMM(uint16_t);
        void LD_REG_REG(uint16_t);
        void OR_REG_REG(uint16_t);
        void AND_REG_REG(uint16_t);
        void XOR_REG_REG(uint16_t);
        void ADD_REG_REG(uint16_t);
        void SUB_REG_REG(uint16_t);
        void SHR_REG_REG(uint16_t);
        void SUBN_REG_REG(uint16_t);
        void SHL_REG_REG(uint16_t);
        void SNE_REG_REG(uint16_t);
        void LD_I_IMM(uint16_t);
        void JP_REG(uint16_t);
        void RND(uint16_t);
        void DRW(uint16_t);
        void SKP(uint16_t);
        void SKNP(uint16_t);
        void LD_DT(uint16_t);
        void LD_REG_KEY(uint16_t);
        void ST_DT(uint16_t);
        void ST_ST(uint16_t);
        void ADD_I_REG(uint16_t);
        void LD_I_REG(uint16_t);
        void LD_I_BCD_REG(uint16_t);
        void ST_REGS(uint16_t);
        void LD_REGS(uint16_t);
        // End instruction implementations

        typedef void (Chip8::*InstructionHandler)(uint16_t opcode);

        struct Instruction {
            uint16_t opcode;
            uint16_t mask;
            InstructionHandler handler;
        };

        Instruction m_instructions[34] = {
            { 0x00E0, 0xFFFF, &Chip8::CLS },
            { 0x00EE, 0xFFFF, &Chip8::RET },
            { 0x1000, 0xF000, &Chip8::JP },
            { 0x2000, 0xF000, &Chip8::CALL },
            { 0x3000, 0xF000, &Chip8::SE_REG_IMM },
            { 0x4000, 0xF000, &Chip8::SNE_REG_IMM },
            { 0x5000, 0xF00F, &Chip8::SE_REG_REG },
            { 0x6000, 0xF000, &Chip8::LD_REG_IMM },
            { 0x7000, 0xF000, &Chip8::ADD_REG_IMM },
            { 0x8000, 0xF00F, &Chip8::LD_REG_REG },
            { 0x8001, 0xF00F, &Chip8::OR_REG_REG },
            { 0x8002, 0xF00F, &Chip8::AND_REG_REG },
            { 0x8003, 0xF00F, &Chip8::XOR_REG_REG },
            { 0x8004, 0xF00F, &Chip8::ADD_REG_REG },
            { 0x8005, 0xF00F, &Chip8::SUB_REG_REG },
            { 0x8006, 0xF00F, &Chip8::SHR_REG_REG },
            { 0x8007, 0xF00F, &Chip8::SUBN_REG_REG },
            { 0x800E, 0xF00F, &Chip8::SHL_REG_REG },
            { 0x9000, 0xF00F, &Chip8::SNE_REG_REG },
            { 0xA000, 0xF000, &Chip8::LD_I_IMM },
            { 0xB000, 0xF000, &Chip8::JP_REG },
            { 0xC000, 0xF000, &Chip8::RND },
            { 0xD000, 0xF000, &Chip8::DRW },
            { 0xE09E, 0xF0FF, &Chip8::SKP },
            { 0xE0A1, 0xF0FF, &Chip8::SKNP },
            { 0xF007, 0xF0FF, &Chip8::LD_DT },
            { 0xF00A, 0xF0FF, &Chip8::LD_REG_KEY },
            { 0xF015, 0xF0FF, &Chip8::ST_DT },
            { 0xF018, 0xF0FF, &Chip8::ST_ST },
            { 0xF01E, 0xF0FF, &Chip8::ADD_I_REG },
            { 0xF029, 0xF0FF, &Chip8::LD_I_REG },
            { 0xF033, 0xF0FF, &Chip8::LD_I_BCD_REG },
            { 0xF055, 0xF0FF, &Chip8::ST_REGS },
            { 0xF065, 0xF0FF, &Chip8::LD_REGS },
        };

        InstructionHandler find_handler(uint16_t opcode);

        void set_vf(bool value) {
            m_registers[15] = value ? 0x01 : 0x00;
        }
};
