//
// Created by alan on 8/17/25.
//

#include "Display.h"

namespace chip8 {

Display::Display() : window(nullptr), renderer(nullptr) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    window = SDL_CreateWindow("CHIP-8",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              DISPLAY_WIDTH * SCALE,
                              DISPLAY_HEIGHT * SCALE,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    // start with a cleared (black) screen
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    std::cout << "Display Successfully initialized\n";
}

Display::~Display() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

void Display::set_pixel(int x, int y, bool on) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        display[y][x] = on;
    }
}

bool Display::get_pixel(int x, int y) const {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        return display[y][x];
    }
    return false;
}

void Display::toggle_pixel(int x, int y) {
     if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        display[y][x] = !display[y][x];
    }
}


void Display::render() const {
    if (!renderer) return;

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            if (display[y][x]) {
                SDL_Rect r = {x * SCALE, y * SCALE, SCALE, SCALE};
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void Display::clear() {
    for (auto & y : display) {
        for (bool & x : y) {
            x = false;
        }
    }
}

void Display::flip_pixel(int x, int y) {
    toggle_pixel(x, y);
}

void Display::draw_sprite(uint8_t x, uint8_t y, const uint8_t* sprite_data, uint8_t height, uint8_t v[]) {
    uint8_t start_x = x % DISPLAY_WIDTH;
    uint8_t start_y = y % DISPLAY_HEIGHT;

    v[0xF] = 0;

    for (int row = 0; row < height; ++row) {
        uint8_t sprite_byte = sprite_data[row];
        int current_y = start_y + row;
        
        if (current_y >= DISPLAY_HEIGHT) continue;

        for (int bit = 0; bit < 8; ++bit) {
            int current_x = start_x + bit;

            if (current_x >= DISPLAY_WIDTH) continue;

            if ((sprite_byte & (0x80 >> bit)) != 0) {
                if (get_pixel(current_x, current_y)) {
                    v[0xF] = 1;
                }
                toggle_pixel(current_x, current_y);
            }
        }
    }
}

} // namespace chip8