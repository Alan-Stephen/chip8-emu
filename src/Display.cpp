//
// Created by alan on 8/17/25.
//

#include "Display.h"
#include <cmath>

namespace chip8 {

Display::Display() : window(nullptr), renderer(nullptr), audio_device(0), audio_phase(0.0), sound_playing(false) {
    if (SDL_WasInit(SDL_INIT_EVERYTHING) == 0) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
            return;
        }
    } else {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            std::cerr << "SDL subsystems could not initialize! SDL_Error: " << SDL_GetError() << "\n";
            return;
        }
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

    SDL_AudioSpec desired_spec, obtained_spec;
    SDL_zero(desired_spec);
    desired_spec.freq = 44100;
    desired_spec.format = AUDIO_S16SYS;
    desired_spec.channels = 1;
    desired_spec.samples = 512;
    desired_spec.callback = audioCallback;
    desired_spec.userdata = this;

    audio_device = SDL_OpenAudioDevice(nullptr, 0, &desired_spec, &obtained_spec, 0);
    if (audio_device == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
    } else {
        SDL_PauseAudioDevice(audio_device, 0);
    }

    std::cout << "Display Successfully initialized\n";
}

Display::~Display() {
    if (audio_device != 0) {
        SDL_PauseAudioDevice(audio_device, 1);
        SDL_CloseAudioDevice(audio_device);
    }
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

void Display::audioCallback(void* userdata, Uint8* stream, int len) {
    Display* display = static_cast<Display*>(userdata);
    if (display) {
        display->generateAudioSamples(stream, len);
    } else {
        SDL_memset(stream, 0, len);
    }
}

void Display::generateAudioSamples(Uint8* stream, int len) {
    if (!sound_playing) {
        SDL_memset(stream, 0, len);
        return;
    }

    const int sample_rate = 44100;
    const double frequency = 440.0;
    const double phase_increment = 2.0 * M_PI * frequency / sample_rate;
    const double amplitude = 0.3;

    Sint16* buffer = reinterpret_cast<Sint16*>(stream);
    int samples = len / sizeof(Sint16);

    for (int i = 0; i < samples; ++i) {
        buffer[i] = static_cast<Sint16>(amplitude * 32767.0 * std::sin(audio_phase));
        audio_phase += phase_increment;
        if (audio_phase >= 2.0 * M_PI) {
            audio_phase -= 2.0 * M_PI;
        }
    }
}

void Display::updateSound(bool playing) {
    sound_playing = playing;
}

} // namespace chip8