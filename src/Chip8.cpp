#include "Chip8.h"
#include <cassert>
#include <iostream>
#include <cstdlib>

Chip8::Chip8(Mem &memory, Display &display, Keypad &keypad)
    : m_memory(memory),
      m_display(display),
      m_keypad(keypad)
{
}

Chip8::InstructionHandler Chip8::find_handler(uint16_t opcode)
{
    for (const auto& insn : m_instructions) {
        if ((opcode & insn.mask) == insn.opcode) {
            return insn.handler;
        }
    }
    return &Chip8::UNKNOWN;
}

void Chip8::step()
{
    m_speaker_on = m_reg_st != 0;

    if (m_key_register.has_value()) {
        if (!m_keypad.has_keydown_event()) {
            return;
        }
        uint8_t keydown_keycode = m_keypad.get_keydown_event();
        m_keypad.clear_keydown_event();
        if (!m_keypad.is_key_down(keydown_keycode)) {
            return;
        }
        m_registers[*m_key_register] = keydown_keycode;
        m_key_register.reset();
    }

    const uint16_t opcode_hi = m_memory.read(m_pc++) << 8;
    const uint16_t opcode_lo = m_memory.read(m_pc++);
    const uint16_t opcode = opcode_hi | opcode_lo;
    (*this.*find_handler(opcode))(opcode);
}

void Chip8::timer_interrupt()
{
    if (m_reg_st > 0) {
        --m_reg_st;
    }
    if (m_reg_dt > 0) {
        --m_reg_dt;
    }
}

void Chip8::UNKNOWN(uint16_t opcode)
{
    std::cout << "Error unknown opcode: " << std::hex << opcode  << " at pc: " << (m_pc - 2) << std::endl;
}

void Chip8::CLS(uint16_t opcode)
{
    m_display.clear();
}

void Chip8::RET(uint16_t opcode)
{
    assert(m_sp > 0);
    m_pc = m_stack[--m_sp];
}

void Chip8::JP(uint16_t opcode)
{
    m_pc = opcode & 0x0FFF;
}

void Chip8::CALL(uint16_t opcode)
{
    assert(m_sp < 16);
    m_stack[m_sp++] = m_pc;
    m_pc = opcode & 0x0FFF;
}

void Chip8::SE_REG_IMM(uint16_t opcode)
{
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;
    if (m_registers[reg] == val) {
        m_pc += 2;
    }
}

void Chip8::SNE_REG_IMM(uint16_t opcode)
{
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t val = (opcode & 0x00FF);
    if (m_registers[reg] != val) {
        m_pc += 2;
    }
}

void Chip8::SE_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    if (m_registers[rx] == m_registers[ry]) {
        m_pc += 2;
    }
}

void Chip8::LD_REG_IMM(uint16_t opcode)
{
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t val = (opcode & 0x00FF);
    m_registers[reg] = val;
}

void Chip8::ADD_REG_IMM(uint16_t opcode)
{
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t val = (opcode & 0x00FF);
    m_registers[reg] += val;
}

void Chip8::LD_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    m_registers[rx] = m_registers[ry];
}

void Chip8::OR_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    m_registers[rx] = m_registers[rx] | m_registers[ry];
}

void Chip8::AND_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    m_registers[rx] = m_registers[rx] & m_registers[ry];
}

void Chip8::XOR_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    m_registers[rx] = m_registers[rx] ^ m_registers[ry];
}

void Chip8::ADD_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    auto orig = m_registers[rx];
    m_registers[rx] = m_registers[rx] + m_registers[ry];
    set_vf(m_registers[rx] < orig);
}

void Chip8::SUB_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    set_vf(m_registers[rx] > m_registers[ry]);
    m_registers[rx] -= m_registers[ry];
}

void Chip8::SHR_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    set_vf(static_cast<bool>(m_registers[rx] & 0x1));
    m_registers[rx] >>= 1;
}

void Chip8::SUBN_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    set_vf(m_registers[ry] > m_registers[rx]);
    m_registers[rx] = m_registers[ry] - m_registers[rx];
}

void Chip8::SHL_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    set_vf(static_cast<bool>(m_registers[rx] & 0x80));
    m_registers[rx] <<= 1;
}

void Chip8::SNE_REG_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    if (m_registers[rx] != m_registers[ry]) {
        m_pc += 2;
    }
}

void Chip8::LD_I_IMM(uint16_t opcode)
{
    m_reg_i = opcode & 0x0FFF;
}

void Chip8::JP_REG(uint16_t opcode)
{
    auto offset = opcode & 0x0FFF;
    m_pc = offset + m_registers[0];
}

void Chip8::RND(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t p1 = (opcode & 0x00FF);
    m_registers[rx] = static_cast<uint8_t>(rand() % 256) & p1;
}

void Chip8::DRW(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    uint8_t ry = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);

    bool collision = false;
    for (auto b = 0; b < n; ++b) {
        auto line = m_memory.read(m_reg_i + b);
        for (uint8_t i = 0; i < 8; ++i) {
            auto x = m_registers[rx] + i;
            auto y = m_registers[ry] + b;

            bool pixel_current = m_display.is_on(x, y);
            bool pixel = line & (1 << (7 - i));
            if (pixel) {
                m_display.draw_xor(x, y);
            }
            collision |= (pixel_current && pixel);
        }
    }
    set_vf(collision);
}

void Chip8::SKP(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    if (m_keypad.is_key_down(m_registers[rx])) {
        m_pc += 2;
    }
}

void Chip8::SKNP(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    if (!m_keypad.is_key_down(m_registers[rx])) {
        m_pc += 2;
    }
}

void Chip8::LD_DT(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    m_registers[rx] = m_reg_dt;
}

void Chip8::LD_REG_KEY(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    m_key_register = rx;
    m_keypad.clear_keydown_event();
}

void Chip8::ST_DT(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    m_reg_dt = m_registers[rx];
}

void Chip8::ST_ST(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    m_reg_st = m_registers[rx];
}

void Chip8::ADD_I_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    m_reg_i += m_registers[rx];
}

void Chip8::LD_I_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    if (m_registers[rx] > 0x0F) {
        m_reg_i = 0x0000;
    } else {
        m_reg_i = m_registers[rx] * 5;
    };
}

void Chip8::LD_I_BCD_REG(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    auto value = m_registers[rx];
    m_memory.write(m_reg_i, value / 100);
    m_memory.write(m_reg_i + 1, (value % 100) / 10);
    m_memory.write(m_reg_i + 2, (value % 10));
}

void Chip8::ST_REGS(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    for (auto i = 0; i <= rx; ++i) {
        m_memory.write(m_reg_i + i, m_registers[i]);
    }
}

void Chip8::LD_REGS(uint16_t opcode)
{
    uint8_t rx = (opcode & 0x0F00) >> 8;
    for (auto i = 0; i <= rx; ++i) {
        m_registers[i] = m_memory.read(m_reg_i + i);
    }
}
