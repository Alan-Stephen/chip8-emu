//
// Created by alan on 8/17/25.
//

#ifndef UNTITLED_INPUT_H
#define UNTITLED_INPUT_H
#include <cstdint>


class Input {
private:
    uint16_t keys = 0;

public:
    void setKeyPressed(uint8_t key) {
        keys |= (1 << key);
    }

    void setKeyReleased(uint8_t key) {
        keys &= ~(1 << key);
    }

    bool isKeyPressed(uint8_t key) const {
        return keys & (1 << key);
    }
};

#endif //UNTITLED_INPUT_H