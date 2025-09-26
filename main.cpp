#include <iostream>
#include "SDL.h"
#include "AddressRegister.h"
#include "Display.h"
#include "Input.h"

int main() {
    char memory[4096];
    char V[16];
    AddressRegister address_register;
    char stack[4096];

    int delay_timer = 0;
    int sound_timer = 0;

    Input input;
    Display display;

    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            // turn on every other pixel in a classic checker pattern
            bool on = ((x + y) % 2 == 0);
            display.setPixel(x, y, on);
        }
    }

    std::cout << "Hello, World!" << std::endl;

    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                return 0;
            }
            if (event.type == SDL_KEYDOWN) {
                printf("Key pressed: %s\n", SDL_GetKeyName(event.key.keysym.sym));
            }
        }
        SDL_Delay(16);
    }
}
