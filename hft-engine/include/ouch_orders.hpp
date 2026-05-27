#pragma once
#include <cstdint>

struct alignas(8) OuchEnterOrderPacket {
    uint64_t order_token;       
    uint32_t shares;            
    uint32_t price;             
    uint16_t stock_locate;      
    uint8_t  message_type;      
    uint8_t  buy_sell_indicator;
    uint8_t  stock[4];          
};

// Allocate a global shared outbound pool matrix of 10 order tracking slots
extern OuchEnterOrderPacket global_outbound_pool[10];
