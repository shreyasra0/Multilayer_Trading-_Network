#include <iostream>
#include "ouch_orders.hpp"

// Define the global storage space block
OuchEnterOrderPacket global_outbound_pool[10];

class OuchOrderEncoder {
public:
    inline void transmit_order_to_exchange(const OuchEnterOrderPacket& order) {
        std::cout << "  [CORE 6 OUTBOUND WIRE] Blasting Binary OUCH Order to Exchange over Kernel-Bypass TCP:\n";
        std::cout << "    Token: " << order.order_token 
                  << " | Side: " << (char)order.buy_sell_indicator 
                  << " | Ticker: ";
        for(int i = 0; i < 4; ++i) std::cout << (char)order.stock[i];
        std::cout << " | Qty: " << order.shares 
                  << " | Scaled Price: " << order.price << "\n";
    }
};

extern "C" void* create_ouch_encoder() {
    return new OuchOrderEncoder();
}

extern "C" void execute_wire_transmission(void* encoder_ptr, const OuchEnterOrderPacket* order) {
    static_cast<OuchOrderEncoder*>(encoder_ptr)->transmit_order_to_exchange(*order);
}
