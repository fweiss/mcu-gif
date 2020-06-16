#include "helpers.h"

Pack::Pack() : packed() {

}

Pack& Pack::operator+(uint16_t element) {
    printf("+ %d\n", element);
    if (packed.size() == 0) {
        packed.push_back(0x04);
    } else {
        packed[0] |= 0x28;
    }
    return *this;
}

Pack& Pack::operator+(Shift bits) {
    printf("<< %d\n", bits.value);
    this->bits = bits.value;
    return *this;
}
