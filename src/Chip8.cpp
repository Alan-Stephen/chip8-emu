//
// Created by alan on 8/17/25.
//

#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

namespace chip8 {

Chip8::Chip8(Display* disp, Input* inp) : display(disp), input(inp) {
    reset();
}

void Chip8::reset() {
    pc = 0x200;
    sp = 0;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;

    for (int i = 0; i < 16; i++) {
        V[i] = 0;
        stack[i] = 0;
    }

    for (int i = 0; i < 4096; i++) {
        memory[i] = 0;
    }

    for (int i = 0; i < 80; i++) {
        memory[0x050 + i] = FONTS[i];
    }
}

bool Chip8::loadProgram(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > 4096 - 0x200) {
        std::cerr << "Program too large" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&memory[0x200]), size);
    return true;
}

uint16_t Chip8::fetch() {
    uint16_t instruction = (static_cast<uint16_t>(memory[pc]) << 8) | static_cast<uint16_t>(memory[pc + 1]);
    pc += 2;
    return instruction;
}

void Chip8::execute(uint16_t instruction) {
    uint16_t opcode = instruction & 0xF000;
    uint8_t x = (instruction & 0x0F00) >> 8;
    uint8_t y = (instruction & 0x00F0) >> 4;
    uint8_t n = instruction & 0x000F;
    uint8_t nn = instruction & 0x00FF;
    uint16_t nnn = instruction & 0x0FFF;

    switch (opcode) {
        case 0x0000:
            if (instruction == 0x00E0) {
                display->clear();
            } else if (instruction == 0x00EE) {
                pc = stack[--sp];
            }
            break;

        case 0x1000:
            pc = nnn;
            break;

        case 0x2000:
            stack[sp++] = pc;
            pc = nnn;
            break;

        case 0x3000:
            if(V[x] == nn) pc += 2;
            break;

        case 0x4000:
            if(V[x] != nn) pc += 2;
            break;

        case 0x5000:
            if(V[x] == V[y]) pc += 2;
            break;

        case 0x9000:
            if(V[x] != V[y]) pc += 2;
            break;

        case 0x6000:
            V[x] = nn;
            break;

        case 0x7000:
            V[x] += nn;
            break;

        case 0x8000:
            switch (n) {
                case 0x0:
                    V[x] = V[y];
                    break;
                case 0x1:
                    V[x] |= V[y];
                    break;
                case 0x2:
                    V[x] &= V[y];
                    break;
                case 0x3:
                    V[x] ^= V[y];
                    break;
                case 0x4: {
                    uint16_t sum = V[x] + V[y];
                    V[0xF] = (sum > 255) ? 1 : 0;
                    V[x] = sum & 0xFF;
                    break;
                }
                case 0x5:
                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    break;
                case 0x6:
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    break;
                case 0x7:
                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    break;
                case 0xE:
                    V[0xF] = V[x] >> 7;
                    V[x] <<= 1;
                    break;
            }
            break;

        case 0xA000:
            I = nnn;
            break;

        case 0xB000:
            pc = nnn + V[0];
            break;

        case 0xC000:
            V[x] = rand() & nn;
            break;

        case 0xD000: {
            uint8_t height = n;
            uint8_t start_x = V[x] % 64;
            uint8_t start_y = V[y] % 32;

            V[0xF] = 0;

            for (int row = 0; row < height; ++row) {
                uint8_t sprite_byte = memory[I + row];
                int current_y = start_y + row;

                if (current_y >= 32) continue;

                for (int bit = 0; bit < 8; ++bit) {
                    int current_x = start_x + bit;

                    if (current_x >= 64) continue;

                    if ((sprite_byte & (0x80 >> bit)) != 0) {
                        if (display->get_pixel(current_x, current_y)) {
                            V[0xF] = 1;
                        }
                        display->toggle_pixel(current_x, current_y);
                    }
                }
            }
            break;
        }

        case 0xE000:
            switch (nn) {
                case 0x9E:
                    if (input->isKeyPressed(V[x])) {
                        pc += 2;
                    }
                    break;
                case 0xA1:
                    if (!input->isKeyPressed(V[x])) {
                        pc += 2;
                    }
                    break;
            }
            break;
        case 0xF000:
            switch (nn) {
                case 0x07:
                    V[x] = delay_timer;
                    break;
                case 0x15:
                    delay_timer = V[x];
                    break;
                case 0x18:
                    sound_timer = V[x];
                    break;
                case 0x1E:
                    I += V[x];
                    break;
                case 0x0A:
                    if (!input->isKeyPressed()) {
                         pc -= 2;
                         break;
                    }
                    V[x] = input->get_key();
                    break;
                    case 0x29:
                        I = 0x050 + (V[x] & 0x0F) * 5;
                        break;
                    case 0x33:
                        memory[I]     = V[x] / 100;
                        memory[I + 1] = (V[x] / 10) % 10;
                        memory[I + 2] = V[x] % 10;
                        break;
                    case 0x55:
                        for (int i = 0; i <= x; ++i) {
                            memory[I + i] = V[i];
                        }
                        break;
                    case 0x65:
                        for (int i = 0; i <= x; ++i) {
                            V[i] = memory[I + i];
                        }
                        break;
            }
            break;
    }
}

void Chip8::step() {
    uint16_t instruction = fetch();
    execute(instruction);
}

void Chip8::updateTimers() {
    if (delay_timer > 0) {
        --delay_timer;
    }
    if (sound_timer > 0) {
        --sound_timer;
    }
}

bool Chip8::isSoundPlaying() const {
    return sound_timer > 0;
}

} // namespace chip8

