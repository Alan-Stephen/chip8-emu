#include <iostream>
#include "SDL.h"
#include "Chip8.h"
#include "Display.h"
#include "Input.h"

static uint8_t mapSDLKeyToChip8(SDL_Keycode key) {
    if (key == SDLK_1) return 0x1;
    if (key == SDLK_2) return 0x2;
    if (key == SDLK_3) return 0x3;
    if (key == SDLK_4) return 0xC;
    if (key == SDLK_q) return 0x4;
    if (key == SDLK_w) return 0x5;
    if (key == SDLK_e) return 0x6;
    if (key == SDLK_r) return 0xD;
    if (key == SDLK_a) return 0x7;
    if (key == SDLK_s) return 0x8;
    if (key == SDLK_d) return 0x9;
    if (key == SDLK_f) return 0xE;
    if (key == SDLK_z) return 0xA;
    if (key == SDLK_x) return 0x0;
    if (key == SDLK_c) return 0xB;
    if (key == SDLK_v) return 0xF;
    return 0xFF;
}

static bool handleEvents(chip8::Input& input) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT ||
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            return false;
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            uint8_t chip8_key = mapSDLKeyToChip8(event.key.keysym.sym);
            if (chip8_key != 0xFF) {
                if (event.type == SDL_KEYDOWN) {
                    input.setKeyPressed(chip8_key);
                } else {
                    input.setKeyReleased(chip8_key);
                }
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <program.ch8>" << std::endl;
        return 1;
    }

    chip8::Input input;
    chip8::Display display;
    chip8::Chip8 emulator(&display, &input);

    if (!emulator.loadProgram(argv[1])) {
        std::cerr << "Failed to load program: " << argv[1] << std::endl;
        return 1;
    }

    std::cout << "CHIP8 Emulator started. Press ESC to exit." << std::endl;

    constexpr uint32_t frame_time_limit = 16;
    constexpr int instructions_per_cycle = 9;

    while (true) {
        Uint32 start_time = SDL_GetTicks();

        if (!handleEvents(input)) {
            return 0;
        }

        for (int i = 0; i < instructions_per_cycle; ++i) {
            emulator.step();
        }

        emulator.updateTimers();
        display.updateSound(emulator.isSoundPlaying());
        display.render();

        Uint32 end_time = SDL_GetTicks();
        Uint32 frame_time = end_time - start_time;
        if (frame_time < frame_time_limit) {
            SDL_Delay(frame_time_limit - frame_time);
        }
    }
}
