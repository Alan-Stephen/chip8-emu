//
// Created by alan on 8/17/25.
//

#ifndef UNTITLED_INPUT_H
#define UNTITLED_INPUT_H
#include <cstdint>

namespace chip8 {

class Input {
    uint16_t keys = 0;

public:
    void setKeyPressed(uint8_t key);
    void setKeyReleased(uint8_t key);
    bool isKeyPressed(uint8_t key) const;
    bool isKeyPressed() const;
    uint8_t getKey() const;
};

} // namespace chip8

#endif //UNTITLED_INPUT_H
