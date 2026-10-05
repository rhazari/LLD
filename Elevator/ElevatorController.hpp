#pragma once

#include "Elevator.hpp"
#include "ElevatorDispatcher.hpp"

class ElevatorController {
private:
    std::vector<std::shared_ptr<Elevator>> elevators;
    std::unique_ptr<DispatchStrategy> strategy;
    std::mutex controllerMutex;

public:
    ElevatorController(int numElevators, std::unique_ptr<DispatchStrategy> dispatchStrategy)
        : strategy(std::move(dispatchStrategy)) 
    {
        for (int i = 1; i <= numElevators; ++i) {
            elevators.push_back(std::make_shared<Elevator>(i, 1)); // Start all elevators at floor 1
        }
    }

    void handleExternalRequest(int floor, Direction direction) {
        std::lock_guard<std::mutex> lock(controllerMutex);
        ExternalRequest req{floor, direction};

        auto selectedElevator = strategy->selectElevator(elevators, req);
        if (selectedElevator) {
            std::cout << "\n[Controller] Assigned External Hall Call (Floor " << floor 
                      << ") -> Elevator " << selectedElevator->getId() << "\n";
            selectedElevator->addRequest(floor);
        }
    }

    void handleInternalRequest(int elevatorId, int destinationFloor) {
        std::lock_guard<std::mutex> lock(controllerMutex);
        for (auto& elevator : elevators) {
            if (elevator->getId() == elevatorId) {
                std::cout << "\n[Controller] Internal Button Pressed in Elevator " << elevatorId 
                          << " -> Floor " << destinationFloor << "\n";
                elevator->addRequest(destinationFloor);
                return;
            }
        }
        std::cout << "[Controller] Elevator ID " << elevatorId << " not found!\n";
    }

    void shutdown() {
        for (auto& elevator : elevators) {
            elevator->stop();
        }
    }
};