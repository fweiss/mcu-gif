#pragma once

#include <queue>
#include <vector>

class Pack {
public:
    Pack();
    Pack &operator+(uint16_t packed);
    operator std::vector<uint8_t> () {
    return packed;
    }
private:
    std::vector<uint8_t> packed;
};
