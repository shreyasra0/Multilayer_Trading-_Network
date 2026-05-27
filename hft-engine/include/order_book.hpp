#pragma once
#include "types.hpp"

constexpr uint32_t PRICE_OFFSET = 1500000; 
constexpr uint32_t BOOK_DEPTH   = 200000;

class FlatLimitOrderBook {
private:
    alignas(64) uint32_t bid_volume_levels[BOOK_DEPTH] = {0};
    alignas(64) uint32_t ask_volume_levels[BOOK_DEPTH] = {0};

public:
    void process_limit_update(const ItchAddOrderPacket& packet);
};
