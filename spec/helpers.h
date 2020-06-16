#pragma once

#include <queue>
#include <vector>

class Unpack {
public:
    Unpack();
    Unpack &operator+(uint16_t packed);
    operator std::vector<uint8_t> () {
//        return unpacked;
//        return std::vector<uint8_t>(unpacked.begin(), unpacked.end());
        return packed;
    }
private:
    std::vector<uint8_t> packed;
};
