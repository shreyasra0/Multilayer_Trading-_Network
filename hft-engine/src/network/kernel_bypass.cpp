#include <iostream>
#include <atomic>
#include <thread>
#include "kernel_bypass.hpp"
#include "ring_buffer.hpp"

// Globally share the pre-allocated memory grid
ItchAddOrderPacket global_dma_pool[10];
extern SpscRingBuffer<ItchAddOrderPacket, 1024> pipeline_queue;

void KernelBypassSimulator::pre_load_simulated_wire_packets() {
    for (int i = 0; i < 10; ++i) {
        global_dma_pool[i].timestamp = 1000000000ULL + i;
        global_dma_pool[i].order_reference = 5000 + i;
        global_dma_pool[i].shares = 100 * (i + 1);
        global_dma_pool[i].price = 1500000 + (i * 10000); 
        global_dma_pool[i].stock_locate = i + 1;
        global_dma_pool[i].tracking_number = 100 + i;
        global_dma_pool[i].message_type = 'A'; 
        global_dma_pool[i].buy_sell_indicator = (i % 2 == 0) ? 'B' : 'S';
        
        global_dma_pool[i].stock[0] = 'A';
        global_dma_pool[i].stock[1] = 'A';
        global_dma_pool[i].stock[2] = 'P';
        global_dma_pool[i].stock[3] = 'L';
        for(int j = 4; j < 8; ++j) {
            global_dma_pool[i].stock[j] = ' ';
        }
    }
    std::cout << "[KERNEL BYPASS] Pre-allocated 10 packets cleanly in global shared memory.\n";
}

void KernelBypassSimulator::blast_packets_into_pipeline() {
    std::cout << "[KERNEL BYPASS] Starting hardware-bypass ingestion loop...\n";
    for (size_t i = 0; i < 10; ++i) {
        // Enqueue the absolute array index integer index slot number
        while (!pipeline_queue.enqueue(i)) {
            std::this_thread::yield(); 
        }
    }
    std::cout << "[KERNEL BYPASS] Successfully injected 10 packet indices into ring buffer.\n";
}
