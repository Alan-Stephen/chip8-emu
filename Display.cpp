//
// Created by alan on 8/17/25.
//

#include "Display.h"
#include <stdexcept>

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
        // leave renderer nullptr; destructor will handle cleanup
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
    // bounds check
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) {
        throw std::out_of_range("Pixel out of bounds");
    }

    display[y][x] = on;

    if (!renderer) {
        // If renderer isn't available, we only update the framebuffer.
        return;
    }

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
}

void Display::render() const {
    if (renderer) {
        SDL_RenderPresent(renderer);
    }
}

void Display::clear() {
    // Reset framebuffer directly to avoid per-pixel bounds checks and repeated draw color changes.
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            display[y][x] = false;
        }
    }

    if (!renderer) {
        return;
    }

    // Clear the renderer to black once.
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
}

void Display::flip_pixel(int x, int y) {
    set_pixel(x, y, !display[y][x]);
}

void Display::draw_sprite(uint8_t x, uint8_t y, const uint8_t * const memory, uint8_t n, uint8_t i, uint8_t v[]) {
    uint8_t vx = v[x] % DISPLAY_WIDTH;
    uint8_t vy = v[y] % DISPLAY_HEIGHT;

    // Reset collision flag
    v[0xF] = 0;

    for (uint8_t j = 0; j < n; ++j) {
        uint8_t sprite_byte = memory[i + j];

        for (uint8_t bit = 0; bit < 8; ++bit) {
            // Check if this bit is set
            if ((sprite_byte & (0x80 >> bit)) != 0) {
                int pixel_x = (vx + bit);
                int pixel_y = (vy + j);

                if (display[pixel_y][pixel_x]) {
                    v[0xF] = 1;
                }

                flip_pixel(pixel_x, pixel_y);
            }
        }
    }
}

} // namespace chip8
