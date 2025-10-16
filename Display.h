//
// Created by alan on 8/17/25.
//

#ifndef UNTITLED_DISPLAY_H
#define UNTITLED_DISPLAY_H

#include "SDL.h"
#include <cstdint>
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
    Display();
    ~Display();

    void set_pixel(int x, int y, bool on);
    void flip_pixel(int x, int y);
    void render() const;
    void clear();
    void draw_sprite(uint8_t x, uint8_t y, const uint8_t memory[], uint8_t height, uint8_t i, uint8_t v[]);
};

}// namespace chip8

#endif //UNTITLED_DISPLAY_H
