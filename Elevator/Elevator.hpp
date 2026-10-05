#pragma once

#include "ElevatorModels.hpp"

class Elevator {
private:
    int id;
    int currentFloor;
    Direction currentDirection;
    DoorState doorState;

    // Requests tracked via std::set to auto-sort and prevent duplicates
    std::set<int> upRequests;
    std::set<int> downRequests;

    std::mutex elevatorMutex;
    std::condition_variable cv;
    std::atomic<bool> running{true};
    std::thread workerThread;

    void processRequests() {
        while (running) {
            std::unique_lock<std::mutex> lock(elevatorMutex);
            
            // Wait until there are requests or system shuts down
            cv.wait(lock, [this]() {
                return !running || !upRequests.empty() || !downRequests.empty();
            });

            if (!running) break;

            if (currentDirection == Direction::UP || currentDirection == Direction::IDLE) {
                if (!upRequests.empty()) {
                    moveToUpTargets();
                } else if (!downRequests.empty()) {
                    currentDirection = Direction::DOWN;
                    moveToDownTargets();
                } else {
                    currentDirection = Direction::IDLE;
                }
            } else if (currentDirection == Direction::DOWN) {
                if (!downRequests.empty()) {
                    moveToDownTargets();
                } else if (!upRequests.empty()) {
                    currentDirection = Direction::UP;
                    moveToUpTargets();
                } else {
                    currentDirection = Direction::IDLE;
                }
            }
        }
    }

    void moveToUpTargets() {
        // Service next higher floor in UP queue
        auto it = upRequests.lower_bound(currentFloor);
        if (it != upRequests.end()) {
            int targetFloor = *it;
            upRequests.erase(it);

            stepToFloor(targetFloor);
        } else {
            // No higher UP targets left, check top-most target in DOWN queue
            currentDirection = Direction::DOWN;
        }
    }

    void moveToDownTargets() {
        // Service next lower floor in DOWN queue
        if (downRequests.empty()) {
            currentDirection = Direction::UP;
            return;
        }

        auto it = downRequests.upper_bound(currentFloor);
        if (it != downRequests.begin()) {
            --it;
            int targetFloor = *it;
            downRequests.erase(it);

            stepToFloor(targetFloor);
        } else {
            currentDirection = Direction::UP;
        }
    }

    void stepToFloor(int targetFloor) {
        currentDirection = (targetFloor > currentFloor) ? Direction::UP : Direction::DOWN;
        
        while (currentFloor != targetFloor) {
            elevatorMutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate movement speed
            elevatorMutex.lock();

            currentFloor += (currentDirection == Direction::UP) ? 1 : -1;
            std::cout << "[Elevator " << id << "] Moved to floor " << currentFloor << "\n";
        }

        // Open doors at target floor
        openAndCloseDoors();
    }

    void openAndCloseDoors() {
        doorState = DoorState::OPEN;
        std::cout << "[Elevator " << id << "] Door OPENED at floor " << currentFloor << "\n";
        
        elevatorMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(800)); // Simulate door dwell time
        elevatorMutex.lock();

        doorState = DoorState::CLOSED;
        std::cout << "[Elevator " << id << "] Door CLOSED at floor " << currentFloor << "\n";
    }

public:
    Elevator(int id, int initialFloor = 1)
        : id(id), currentFloor(initialFloor), currentDirection(Direction::IDLE), doorState(DoorState::CLOSED) {
        workerThread = std::thread(&Elevator::processRequests, this);
    }

    ~Elevator() {
        stop();
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(elevatorMutex);
            running = false;
        }
        cv.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    void addRequest(int targetFloor) {
        std::lock_guard<std::mutex> lock(elevatorMutex);
        if (targetFloor == currentFloor && doorState == DoorState::OPEN) {
            return;
        }

        if (targetFloor >= currentFloor) {
            upRequests.insert(targetFloor);
        } else {
            downRequests.insert(targetFloor);
        }

        if (currentDirection == Direction::IDLE) {
            currentDirection = (targetFloor >= currentFloor) ? Direction::UP : Direction::DOWN;
        }

        std::cout << "[Elevator " << id << "] Added request for floor " << targetFloor << "\n";
        cv.notify_one();
    }

    // Getters for strategy calculations
    int getId() const { return id; }
    int getCurrentFloor() const { return currentFloor; }
    Direction getCurrentDirection() const { return currentDirection; }
};