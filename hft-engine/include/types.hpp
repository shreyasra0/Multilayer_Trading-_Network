#pragma once
#include <cstdint>

struct alignas(8) ItchAddOrderPacket {
    uint64_t timestamp;         
    uint64_t order_reference;   
    uint32_t shares;            
    uint32_t price;             
    uint16_t stock_locate;      
    uint16_t tracking_number;   
    uint8_t  message_type;      
    uint8_t  buy_sell_indicator;
    uint8_t  stock[8];          
    uint8_t  pad[2];            
};
