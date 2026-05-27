#include <iostream>
#include <thread>
#include <pthread.h>
#include <atomic>
#include <chrono>
#include <memory>

#ifdef __APPLE__
#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#else
#include <sched.h>
#endif

#include "types.hpp"
#include "ring_buffer.hpp"
#include "order_book.hpp"
#include "kernel_bypass.hpp"
#include "ouch_orders.hpp"

SpscRingBuffer<ItchAddOrderPacket, 1024> inbound_queue;
SpscRingBuffer<OuchEnterOrderPacket, 1024> outbound_queue;

std::atomic<bool> engine_running{true};
uint64_t unique_token_generator = 90001;

extern "C" void* create_ouch_encoder();
extern "C" void execute_wire_transmission(void* encoder_ptr, const OuchEnterOrderPacket* order);
extern "C" void* create_session_manager();
extern "C" bool validate_session_sequence(void* mgr_ptr, const ItchAddOrderPacket* packet);

void pin_thread_to_hardware_core(int core_id) {
#ifdef __APPLE__
    thread_affinity_policy_data_t policy = { core_id };
    thread_port_t mach_thread = pthread_mach_thread_np(pthread_self());
    thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT);
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
#endif
}

void network_ingest_producer_loop() {
    pin_thread_to_hardware_core(2); 
    std::cout << "[PRODUCER] Ingest thread spinning on Core 2...\n";

    KernelBypassSimulator nic_simulator;
    nic_simulator.pre_load_simulated_wire_packets();
    
    // Explicitly modify the shared memory segment sequence fields to match the protocol specification
    global_dma_pool[0].tracking_number = 1;
    global_dma_pool[1].tracking_number = 2;
    global_dma_pool[2].tracking_number = 3;

    std::cout << "[KERNEL BYPASS] Injecting wire feed with a simulated network packet drop...\n";
    
    // Step A: Push Packet 1
    inbound_queue.enqueue(0);
    
    // Step B: Intentionally skip Packet 2 (Index 1) and blast forward to Packet 3 (Index 2)
    inbound_queue.enqueue(2);
    
    // Step C: Playback the missing Packet 2 (Index 1) over the out-of-band replay pipeline
    inbound_queue.enqueue(1);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    engine_running.store(false);
}

void strategy_engine_consumer_loop() {
    pin_thread_to_hardware_core(4); 
    std::cout << "[CONSUMER] Strategy & Sync Engine active on Core 4...\n";

    auto local_order_book = std::make_unique<FlatLimitOrderBook>();
    void* session_mgr = create_session_manager();
    size_t target_pool_index = 0;

    while (engine_running.load(std::memory_order_relaxed) || inbound_queue.dequeue(target_pool_index)) {
        if (target_pool_index < 10) {
            const auto& incoming_packet = global_dma_pool[target_pool_index];
            std::cout << "[CORE 4 INGEST] Extracted Packet Sequence ID: " << incoming_packet.tracking_number << "\n";

            if (validate_session_sequence(session_mgr, &incoming_packet)) {
                std::cout << "  [CORE 4 ACTION] Sequence Approved. Routing to Matching Engine:\n";
                local_order_book->process_limit_update(incoming_packet);
            } else {
                std::cout << "  ❌ [CORE 4 BLOCK] Sequence Rejected by Session Layer. Order Book Protection Active.\n";
            }
            target_pool_index = 999; 
        }
    }
}

int main() {
    std::cout << "Starting HFT Core with Integrated Layer 5 Resynchronization Engine...\n";

    std::thread producer(network_ingest_producer_loop);
    std::thread strategy(strategy_engine_consumer_loop);

    producer.join();
    strategy.join();

    std::cout << "\nExecution complete. All network states traed.\n";
    return 0;
}
