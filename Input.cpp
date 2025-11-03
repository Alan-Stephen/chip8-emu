//
// Created by alan on 8/17/25.
//

#include "Input.h"

namespace chip8 {

void Input::setKeyPressed(uint8_t key) {
    keys |= (1 << key);
}

void Input::setKeyReleased(uint8_t key) {
    keys &= ~(1 << key);
}

bool Input::isKeyPressed(uint8_t key) const {
    return keys & (1 << key);
}

bool Input::isKeyPressed() const {
    return keys != 0;
}

uint8_t Input::get_key() const {
    for (uint8_t i = 0; i < 16; ++i) {
        if (keys & (1 << i)) {
            return i;
        }
    }
    return 0;
}

} // namespace chip8
