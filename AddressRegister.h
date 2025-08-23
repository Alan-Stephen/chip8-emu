//
// Created by alan on 8/17/25.
//

#ifndef ADDRESSREGISTER_H
#define ADDRESSREGISTER_H
#include <cstdint>


class AddressRegister {
    uint16_t value = 0;
    static constexpr uint16_t MASK = 0x0FFF;

public:
    constexpr AddressRegister() = default;

    constexpr explicit AddressRegister(uint16_t v) : value(v & MASK) {
    }

    // assignment
    AddressRegister &operator=(uint16_t v) {
        value = v & MASK;
        return *this;
    }

    // implicit conversion to uint16_t
    constexpr operator uint16_t() const { return value & MASK; }

    // arithmetic helpers (wrap automatically)
    AddressRegister &operator+=(uint16_t v) {
        value = (value + v) & MASK;
        return *this;
    }

    AddressRegister &operator-=(uint16_t v) {
        value = (value - v) & MASK;
        return *this;
    }

    // explicit accessors
    constexpr uint16_t get() const { return value & MASK; }
    void set(uint16_t v) { value = v & MASK; }
};


#endif //ADDRESSREGISTER_H