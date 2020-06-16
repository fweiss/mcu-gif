#include "helpers.h"

static const bool debug = false;

Pack::Pack() : packed() {

}

Pack& Pack::operator+(uint16_t element) {
    if (debug) printf("+ %d\n", element);

    if (packed.size() == 0) {
        packed.push_back(0x04);
        onDeckBits = bits;
    } else {
        packed[0] |=  element << onDeckBits ; //0x28;
    }


    return *this;
}

Pack& Pack::operator+(Shift bits) {
    if (debug) printf("<< %d\n", bits.value);
    this->bits = bits.value;
    return *this;
}
