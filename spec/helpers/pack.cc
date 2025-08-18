#include "pack.h"

#include <algorithm>

static const bool debug = false;

Pack::Pack() : packed() {

}

void Pack::reset() {
    packed.clear();
    codeBits = 3;
    codeMask = 0x07;
    onDeckBits = 0;
    onDeck = 0;
}

Pack& Pack::operator+(uint16_t element) {
    if (debug) printf("+ %d\n", element);

    uint8_t elementBits = codeBits;
    while (elementBits > 0) {
        uint8_t shiftBits = std::min(elementBits, (uint8_t)(8 - onDeckBits));
        onDeck |= (element & codeMask) << onDeckBits; // onDeck only takes 8 bits
        element >>= shiftBits;
        elementBits -= shiftBits;
        onDeckBits += shiftBits;
        if (onDeckBits == 8) {
            packed.push_back(onDeck);
            onDeck = 0;
            onDeckBits = 0;
        }
    }

    return *this;
}

//fixme dead code?
Pack::operator std::vector<uint8_t> () {
    if (onDeckBits > 0) {
        packed.push_back(onDeck);
    }
    return packed;
}

Pack::operator PackedSubBlock() {
    if (onDeckBits > 0) {
        packed.push_back(onDeck);
    }
    return packed;  
}

Pack& Pack::operator+(Shift bits) {
    if (debug) printf("<< %d\n", bits.value);
    this->codeBits = bits.value;
    this->codeMask = (1 << codeBits) - 1;
    return *this;
}
