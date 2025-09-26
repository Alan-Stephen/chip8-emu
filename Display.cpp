//
// Created by alan on 8/17/25.
//

#include "Display.h"

void Display::setPixel(int x, int y, bool on) {
    // bounds check
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) {
        throw std::out_of_range("Pixel out of bounds");
    }

    display[y][x] = on;

    if (on) {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    } else {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    }

    // draw scaled rectangle
    SDL_Rect r;
    r.x = x * SCALE;
    r.y = y * SCALE;
    r.w = SCALE;
    r.h = SCALE;
    SDL_RenderFillRect(renderer, &r);

    SDL_RenderPresent(renderer);
}