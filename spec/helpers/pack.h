#pragma once

#include <vector>
#include <string>

class Shift {
public:
    uint8_t value;
    Shift(uint8_t shift) {
        this->value = shift;
    }
};

// partial proxy a std::vector with uint8_t size
class PackedSubBlock {
public:
    PackedSubBlock();
    PackedSubBlock(const std::vector<uint8_t> &data) { vector = data; }
    // void operator=(std::vector<uint8_t> input) { vector = input; };

    uint8_t *data() { return vector.data(); }
    // todo guarantee that size <256
    uint8_t size() { return static_cast<uint8_t>(vector.size()); }
private:
    std::vector<uint8_t> vector;
};

class Pack {
public:
    Pack();
    operator std::vector<uint8_t> ();
    operator PackedSubBlock();
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

std::string dump(std::vector<uint8_t> p);
