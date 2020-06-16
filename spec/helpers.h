#pragma once

#include <vector>

class Pack {
public:
    Pack();
    void reset() {
        packed.clear();
    }
    Pack &operator+(uint16_t packed);
    operator std::vector<uint8_t> () {
        return packed;
    }
private:
    std::vector<uint8_t> packed;
};
