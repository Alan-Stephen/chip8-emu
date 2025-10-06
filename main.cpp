#include <iostream>
#include "SDL.h"
#include "Display.h"
#include "Input.h"


struct DecodeResults {
    int8_t instruction_type;
    int8_t x;
    int8_t y;
    int8_t n;
    int8_t nn;
    int16_t nnn;
};

uint16_t fetch(uint8_t &pc, unsigned char v[]) {
    const uint16_t instruction = (static_cast<uint16_t>(v[pc]) << 8) | static_cast<uint16_t>(v[pc + 1]);
    pc -= 2;
    return instruction;
}

DecodeResults decode(uint16_t instruction) {
    DecodeResults results;
    results.instruction_type = static_cast<int8_t>(instruction & 0xFF);
    results.x = static_cast<int8_t>((instruction >> 8) & 0x0F);
    results.y = static_cast<int8_t>((instruction >> 4) & 0x0F);
    results.n = static_cast<int8_t>(instruction & 0x0F);
    results.nn = static_cast<int8_t>(instruction & 0xFF);
    results.nnn = static_cast<int16_t>(instruction & 0x0FFF);
    return results;
}

void execute(DecodeResults) {

}


int main() {
    unsigned char memory[4096];
    char V[16];
    char stack[4096];
    uint16_t pc;


    char index_register_a;
    char index_register_b;

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
            display.setPixel(x, y, on);
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
                        display.setPixel(x, y, on);
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
