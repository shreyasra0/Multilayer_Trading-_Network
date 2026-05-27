#pragma once
#include <atomic>
#include <cstddef>

template <typename T, size_t Capacity>
class SpscRingBuffer {
private:
    // Store simple primitive index numbers instead of hardware pointers
    alignas(64) size_t storage[Capacity];
    alignas(64) std::atomic<size_t> head{0};
    alignas(64) std::atomic<size_t> tail{0};

public:
    bool enqueue(size_t index) {
        const size_t current_head = head.load(std::memory_order_relaxed);
        const size_t current_tail = tail.load(std::memory_order_acquire);

        if ((current_head + 1) % Capacity == current_tail) {
            return false; 
        }

        storage[current_head] = index;
        head.store((current_head + 1) % Capacity, std::memory_order_release);
        return true;
    }

    bool dequeue(size_t& index) {
        const size_t current_tail = tail.load(std::memory_order_relaxed);
        const size_t current_head = head.load(std::memory_order_acquire);

        if (current_tail == current_head) {
            return false; 
        }

        index = storage[current_tail];
        tail.store((current_tail + 1) % Capacity, std::memory_order_release);
        return true;
    }
};
