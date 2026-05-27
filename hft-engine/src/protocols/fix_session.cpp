#include <iostream>
#include "session_states.hpp"
#include "types.hpp"

class ItchSessionManager {
private:
    SessionTracker tracker;
    alignas(64) ItchAddOrderPacket gap_stash_buffer[10];

public:
    inline bool process_session_sequence(const ItchAddOrderPacket& packet) {
        uint64_t current_seq = packet.tracking_number;

        if (tracker.current_state == SessionState::SYNCHRONIZED && current_seq == tracker.next_expected_sequence) {
            tracker.next_expected_sequence++;
            return true; 
        }

        if (tracker.current_state == SessionState::SYNCHRONIZED && current_seq > tracker.next_expected_sequence) {
            std::cout << "  ⚠️ [L5 SESSION GAP] Sequence Gap Detected! Expected: " 
                      << tracker.next_expected_sequence << " | Received: " << current_seq << "\n";
            
            tracker.current_state = SessionState::GAP_DETECTED;
            tracker.total_dropped_packets = (current_seq - tracker.next_expected_sequence);

            gap_stash_buffer[current_seq % 10] = packet;

            std::cout << "    [L5 RECOVERY] Triggering Asynchronous Out-of-Band TCP Replay Request...\n";
            tracker.current_state = SessionState::RECOVERING;
            return false; 
        }

        if (tracker.current_state == SessionState::RECOVERING) {
            if (current_seq == (tracker.next_expected_sequence)) {
                std::cout << "    [L5 RECOVERY SUCCESS] Received Missed Packet ID: " << current_seq << " via Replay Link.\n";
                tracker.total_replayed_packets++;
                tracker.next_expected_sequence++;
                
                if (tracker.total_replayed_packets >= tracker.total_dropped_packets) {
                    std::cout << "  ✅ [L5 SESSION] All gaps patched. Session Resynchronized.\n";
                    tracker.current_state = SessionState::SYNCHRONIZED;
                }
                return true;
            }
        }

        return false;
    }
};

extern "C" void* create_session_manager() {
    return new ItchSessionManager();
}

extern "C" bool validate_session_sequence(void* mgr_ptr, const ItchAddOrderPacket* packet) {
    return static_cast<ItchSessionManager*>(mgr_ptr)->process_session_sequence(*packet);
}
