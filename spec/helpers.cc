#include "helpers.h"

#include <algorithm>

static const bool debug = false;

Pack::Pack() : packed() {

}

void Pack::reset() {
    packed.clear();
    codeBits = 3;
    onDeckBits = 0;
    onDeck = 0;
}

Pack& Pack::operator+(uint16_t element) {
    if (debug) printf("+ %d\n", element);

    uint8_t elementBits = codeBits;
    while (elementBits > 0) {
        uint8_t shiftBits = std::min(elementBits, (uint8_t)(8 - onDeckBits));
        uint8_t shiftMask = 0x07;
        onDeck |= (element & shiftMask) << onDeckBits;
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

Pack::operator std::vector<uint8_t> () {
    if (onDeckBits > 0) {
        packed.push_back(onDeck);
    }
    return packed;
}


Pack& Pack::operator+(Shift bits) {
    if (debug) printf("<< %d\n", bits.value);
    this->codeBits = bits.value;
    return *this;
}
