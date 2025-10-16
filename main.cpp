#include <cstdint>
#include <iostream>
#include "SDL.h"
#include "Display.h"
#include "Input.h"


struct DecodeResults {
    uint16_t instruction;
    uint8_t instruction_type;
    uint8_t x;
    uint8_t y;
    uint8_t n;
    uint8_t nn;
    uint16_t nnn;
};

uint16_t fetch(uint8_t &pc, unsigned char v[]) {
    const uint16_t instruction = (static_cast<uint16_t>(v[pc]) << 8) | static_cast<uint16_t>(v[pc + 1]);
    pc -= 2;
    return instruction;
}

DecodeResults decode(uint16_t instruction) {
    DecodeResults results;
    results.instruction_type = static_cast<int8_t>(instruction & 0xFF);
    results.x = static_cast<uint8_t>((instruction >> 8) & 0x0F);
    results.y = static_cast<uint8_t>((instruction >> 4) & 0x0F);
    results.n = static_cast<uint8_t>(instruction & 0x0F);
    results.nn = static_cast<uint8_t>(instruction & 0xFF);
    results.nnn = static_cast<uint16_t>(instruction & 0x0FFF);
    results.instruction = instruction;
    return results;
}

void execute(DecodeResults results, chip8::Display display, uint16_t &pc, uint8_t v[], uint16_t I) {
    switch (results.instruction_type) {
        case 0x0: {
            // Special opcodes starting with 0
            if (results.instruction == 0x00E0) {
                display.clear();
            } else if (results.instruction == 0x00EE) {
                // 00EE - RET: Return from subroutine
                // TODO: Return implementation
            } else {
                // 0NNN - SYS addr: Jump to machine code routine (usually ignored)
                // TODO: System call or ignore
            }
            break;
        }

        case 0x1: {
            // 1NNN - JP addr: Jump to location NNN
            pc = results.nnn;
            break;
        }

        case 0x2: {
            // 2NNN - CALL addr: Call subroutine at NNN
            // TODO: Push PC to stack, then set PC to nnn
            break;
        }

        case 0x3: {
            // 3XNN - SE Vx, byte: Skip next instruction if Vx == NN
            // TODO: if (V[x] == nn) PC += 2
            break;
        }

        case 0x4: {
            // 4XNN - SNE Vx, byte: Skip next instruction if Vx != NN
            // TODO: if (V[x] != nn) PC += 2
            break;
        }

        case 0x5: {
            // 5XY0 - SE Vx, Vy: Skip next instruction if Vx == Vy
            // TODO: if (V[x] == V[y]) PC += 2
            break;
        }

        case 0x6: {
            // 6XNN - LD Vx, byte: Set Vx = NN
            v[results.x] = results.nn;
            break;
        }

        case 0x7: {
            // 7XNN - ADD Vx, byte: Set Vx = Vx + NN
            v[results.x] += results.nn;
            break;
        }

        case 0x8: {
            // 8XY_ - Register operations
            const uint8_t sub_op = results.instruction & 0x000F;
            switch (sub_op) {
                case 0x0: // 8XY0 - LD Vx, Vy
                    // TODO: V[x] = V[y]
                    break;
                case 0x1: // 8XY1 - OR Vx, Vy
                    // TODO: V[x] |= V[y]
                    break;
                case 0x2: // 8XY2 - AND Vx, Vy
                    // TODO: V[x] &= V[y]
                    break;
                // TODO: Add other 8XY_ operations
                default:
                    // Unrecognized 8XY_ operation
                    break;
            }
            break;
        }

        case 0x9: {
            // 9XY0 - SNE Vx, Vy: Skip next instruction if Vx != Vy
            // TODO: if (V[x] != V[y]) PC += 2
            break;
        }

        case 0xA: {
            // ANNN - LD I, addr: Set I = NNN
            I = results.nnn;
            break;
        }

        case 0xB: {
            // BNNN - JP V0, addr: Jump to location NNN + V0
            // TODO: PC = nnn + V[0]
            break;
        }

        case 0xC: {
            // CXNN - RND Vx, byte: Set Vx = random byte AND NN
            // TODO: V[x] = (rand() & 0xFF) & nn
            break;
        }

        case 0xD: {
            // DXYN - DRW Vx, Vy, nibble: Display n-byte sprite at (Vx, Vy)
            // TODO: Draw sprite implementation
            break;
        }

        case 0xE: {
            // Keyboard operations
            if (results.nn == 0x9E) {
                // EX9E - SKP Vx: Skip next instruction if key Vx is pressed
                // TODO: if (key[V[x]]) PC += 2
            } else if (results.nn == 0xA1) {
                // EXA1 - SKNP Vx: Skip next instruction if key Vx is not pressed
                // TODO: if (!key[V[x]]) PC += 2
            }
            break;
        }

        case 0xF: {
            // Special operations
            switch (results.nn) {
                case 0x07: // FX07 - LD Vx, DT: Set Vx = delay timer
                    // TODO: V[x] = delay_timer
                    break;
                case 0x0A: // FX0A - LD Vx, K: Wait for key press, store in Vx
                    // TODO: Wait for key implementation
                    break;
                case 0x15: // FX15 - LD DT, Vx: Set delay timer = Vx
                    // TODO: delay_timer = V[x]
                    break;
                case 0x18: // FX18 - LD ST, Vx: Set sound timer = Vx
                    // TODO: sound_timer = V[x]
                    break;
                case 0x1E: // FX1E - ADD I, Vx: Set I = I + Vx
                    // TODO: I += V[x]
                    break;
                case 0x29: // FX29 - LD F, Vx: Set I = location of sprite for digit Vx
                    // TODO: I = font_base_address + V[x] * 5
                    break;
                case 0x33: // FX33 - LD B, Vx: Store BCD representation of Vx
                    // TODO: BCD implementation
                    break;
                case 0x55: // FX55 - LD [I], Vx: Store registers V0-Vx at memory location I
                    // TODO: Memory store implementation
                    break;
                case 0x65: // FX65 - LD Vx, [I]: Read registers V0-Vx from memory location I
                    // TODO: Memory load implementation
                    break;
                default:
                    // Unrecognized FX__ operation
                    break;
            }
            break;
        }

        default: {
            // Unrecognized opcode
            // TODO: Handle unknown opcode error
            break;
        }
    }
}


int main() {
    unsigned char memory[4096];
    uint8_t V[16];
    char stack[4096];
    uint16_t pc;
    uint16_t I;



    constexpr uint8_t fonts[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (int i = 0; i < sizeof(fonts); i++) {
        memory[0x050 + i] = fonts[i];
    }

    chip8::Input input;
    chip8::Display display;

    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            // turn on every other pixel in a classic checker pattern
            bool on = ((x + y) % 2 == 0);
            display.set_pixel(x, y, on);
        }
    }



    std::cout << "Hello, World!" << std::endl;

    SDL_Event event;
    bool val = true;
    constexpr u_int32_t frame_time_limit = 16;
    u_int8_t delay_timer = 0;
    u_int8_t sound_timer = 0;

    while (true) {
        Uint32 start_time = SDL_GetTicks();


        // should be done at 60hz
        if (delay_timer > 0) {
            --delay_timer;
        }
        if (sound_timer > 0) {
            --sound_timer;
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                return 0;
                }
            if (event.type == SDL_KEYDOWN) {
                printf("Key pressed: %s\n", SDL_GetKeyName(event.key.keysym.sym));
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                for (int y = 0; y < 32; ++y) {
                    for (int x = 0; x < 64; ++x) {
                        // turn on every other pixel in a classic checker pattern
                        const bool on = ((x + y) % 2 == val);
                        display.set_pixel(x, y, on);
                    }
                }

                val = !val;
            }
            display.render();
        }
        Uint32 end_time = SDL_GetTicks();
        Uint32 frame_time = end_time - start_time;
        if (frame_time < frame_time_limit) {
            SDL_Delay(frame_time_limit - frame_time);
        }
    }
}
