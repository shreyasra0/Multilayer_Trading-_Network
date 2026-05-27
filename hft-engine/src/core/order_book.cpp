#include <iostream>
#include "order_book.hpp"

void FlatLimitOrderBook::process_limit_update(const ItchAddOrderPacket& packet) {
    if (packet.price < PRICE_OFFSET || packet.price >= (PRICE_OFFSET + BOOK_DEPTH)) {
        return;
    }
    uint32_t target_index = packet.price - PRICE_OFFSET;

    if (packet.buy_sell_indicator == 'B') {
        bid_volume_levels[target_index] += packet.shares;
        std::cout << "  [ORDER BOOK MATRIX] Updated Bid Level at Scaled Price " << packet.price 
                  << " -> Aggregated Vol: " << bid_volume_levels[target_index] << "\n";
    } else if (packet.buy_sell_indicator == 'S') {
        ask_volume_levels[target_index] += packet.shares;
        std::cout << "  [ORDER BOOK MATRIX] Updated Ask Level at Scaled Price " << packet.price 
                  << " -> Aggregated Vol: " << ask_volume_levels[target_index] << "\n";
    }
}
