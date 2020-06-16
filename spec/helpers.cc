#include "helpers.h"

Unpack::Unpack() : packed() {

}

Unpack& Unpack::operator+(uint16_t element) {
    if (packed.size() == 0) {
        packed.push_back(0x04);
    } else {
        packed[0] |= 0x28;
    }
    return *this;
}
