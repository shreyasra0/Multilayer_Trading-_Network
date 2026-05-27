#include "types.hpp"
#include <iostream>

class SbeDecoder {
public:
    static inline const ItchAddOrderPacket* parse_add_order(const uint8_t* raw_buffer) {
        return reinterpret_cast<const ItchAddOrderPacket*>(raw_buffer);
    }
};
