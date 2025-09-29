//
// Created by alan on 8/17/25.
//

#ifndef UNTITLED_DISPLAY_H
#define UNTITLED_DISPLAY_H

#include "SDL.h"
#include <iostream>

namespace chip8 {

class Display {
private:
    static constexpr int DISPLAY_WIDTH = 64;
    static constexpr int DISPLAY_HEIGHT = 32;
    static constexpr int SCALE = 10;

    SDL_Window *window;
    SDL_Renderer *renderer;
    bool display[DISPLAY_HEIGHT][DISPLAY_WIDTH] = {false};

public:
    Display() {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  DISPLAY_WIDTH * SCALE, DISPLAY_HEIGHT * SCALE, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
            return;
        }

        if (!renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
            return;
        }

        std::cout << "Display Successfully initialized\n";
    }

    ~Display() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void setPixel(int x, int y, bool on);

    void render() const;
};

} // namespace chip8

#endif //UNTITLED_DISPLAY_H