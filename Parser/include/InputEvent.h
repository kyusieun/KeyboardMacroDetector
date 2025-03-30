#pragma once
#include <chrono>

enum class EventType {
    KEY_DOWN,
    KEY_UP
};

struct InputEvent {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    EventType type;
    unsigned int keyCode;
}; 