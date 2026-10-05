#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>

enum class Direction { UP, DOWN, IDLE };
enum class DoorState { OPEN, CLOSED };

// External request from a floor (Hall Call)
struct ExternalRequest {
    int floor;
    Direction direction;

    bool operator<(const ExternalRequest& other) const {
        return floor < other.floor;
    }
};

// Internal request inside an elevator cabin
struct InternalRequest {
    int destinationFloor;
};