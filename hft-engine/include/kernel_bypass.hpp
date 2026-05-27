#pragma once
#include "types.hpp"

// Declare the memory pool globally so both cores can access it natively
extern ItchAddOrderPacket global_dma_pool[10];

class KernelBypassSimulator {
public:
    void pre_load_simulated_wire_packets();
    void blast_packets_into_pipeline();
};
