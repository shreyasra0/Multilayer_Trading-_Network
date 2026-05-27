#pragma once
#include <cstdint>

enum class SessionState {
    SYNCHRONIZED,
    GAP_DETECTED,
    RECOVERING,
    CRITICAL_FAULT
};

struct alignas(64) SessionTracker {
    uint64_t next_expected_sequence{1};
    SessionState current_state{SessionState::SYNCHRONIZED};
    uint64_t total_dropped_packets{0};
    uint64_t total_replayed_packets{0};
};
