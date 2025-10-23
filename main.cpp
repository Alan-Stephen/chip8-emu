#include <iostream>
#include <fstream>
#include "SDL.h"
#include "AddressRegister.h"
#include "Display.h"
#include "Input.h"

const uint8_t fonts[80] = {
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

class CHIP8 {
private:
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t stack[16];
    uint8_t sp;
    uint16_t pc;
    AddressRegister I;
    uint8_t delay_timer;
    uint8_t sound_timer;
    
    chip8::Display* display;
    chip8::Input* input;

public:
    CHIP8(chip8::Display* disp, chip8::Input* inp) : display(disp), input(inp) {
        reset();
    }
    
    void reset() {
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
            memory[0x050 + i] = fonts[i];
        }
    }
    
    bool loadProgram(const std::string& filename) {
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
    
    uint16_t fetch() {
        uint16_t instruction = (static_cast<uint16_t>(memory[pc]) << 8) | static_cast<uint16_t>(memory[pc + 1]);
        pc += 2;
        return instruction;
    }
    
    void execute(uint16_t instruction) {
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
                
            case 0x6000:
                V[x] = nn;
                break;
                
            case 0x7000:
                V[x] += nn;
                break;
                
            case 0xA000:
                I = nnn;
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
        }
    }
    
    void step() {
        uint16_t instruction = fetch();
        execute(instruction);
    }
    
    void updateTimers() {
        if (delay_timer > 0) {
            --delay_timer;
        }
        if (sound_timer > 0) {
            --sound_timer;
        }
    }
};

int main() {
    chip8::Input input;
    chip8::Display display;
    
    CHIP8 emulator(&display, &input);
    
    if (!emulator.loadProgram("ibm-Logo.ch8")) {
        std::cerr << "Failed to load IBM Logo program" << std::endl;
        return 1;
    }
    
    std::cout << "CHIP8 Emulator started. Press ESC to exit." << std::endl;

    SDL_Event event;
    constexpr uint32_t frame_time_limit = 16;
    uint32_t timer_counter = 0;

    while (true) {
        Uint32 start_time = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                return 0;
            }
        }
        
        emulator.step();
        display.render();
        
        timer_counter++;
        if (timer_counter >= 9) {
            emulator.updateTimers();
            timer_counter = 0;
        }
        
        Uint32 end_time = SDL_GetTicks();
        Uint32 frame_time = end_time - start_time;
        if (frame_time < frame_time_limit) {
            SDL_Delay(frame_time_limit - frame_time);
        }
    }
}
