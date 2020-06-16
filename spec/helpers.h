#pragma once

#include <vector>

class Shift {
public:
    uint8_t value;
    Shift(uint8_t shift) {
        this->value = shift;
    }
};

class Pack {
public:
    Pack();
    operator std::vector<uint8_t> () {
        return packed;
    }
    void reset() {
        packed.clear();
        bits = 3;
        onDeckBits = 0;
    }
    // normal code values
    Pack &operator+(uint16_t packed);
    // shift change number of bits
    Pack &operator+(Shift bits);
private:
    std::vector<uint8_t> packed;
    uint8_t bits;

    uint8_t onDeckBits;
};
