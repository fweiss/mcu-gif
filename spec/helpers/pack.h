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
    operator std::vector<uint8_t> ();
    void reset();
    Pack &operator+(uint16_t packed); // normal code values
    Pack &operator+(Shift bits); // shift change number of bits
private:
    std::vector<uint8_t> packed;

    uint8_t codeBits;
    uint16_t codeMask;
    uint8_t onDeckBits;
    uint8_t onDeck;
};
