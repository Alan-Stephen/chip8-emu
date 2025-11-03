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

    SDL_AudioDeviceID audio_device;
    double audio_phase;
    bool sound_playing;

    static void audioCallback(void* userdata, Uint8* stream, int len);
    void generateAudioSamples(Uint8* stream, int len);

public:
    Display();
    ~Display();

    void setPixel(int x, int y, bool on);
    void flipPixel(int x, int y);
    void render() const;
    void clear();
    bool getPixel(int x, int y) const;
    void togglePixel(int x, int y);
    void drawSprite(uint8_t x, uint8_t y, const uint8_t* sprite_data, uint8_t height, uint8_t v[]);
    void updateSound(bool playing);
};

}// namespace chip8

#endif //UNTITLED_DISPLAY_H