#pragma once

#include "Elevator.hpp"
#include <cmath>
#include <climits>

class DispatchStrategy {
public:
    virtual ~DispatchStrategy() = default;
    virtual std::shared_ptr<Elevator> selectElevator(
        const std::vector<std::shared_ptr<Elevator>>& elevators, 
        const ExternalRequest& request) = 0;
};

// Nearest Elevator First Strategy: Evaluates distance + directional alignment
class NearestElevatorStrategy : public DispatchStrategy {
public:
    std::shared_ptr<Elevator> selectElevator(
        const std::vector<std::shared_ptr<Elevator>>& elevators, 
        const ExternalRequest& request) override 
    {
        std::shared_ptr<Elevator> bestElevator = nullptr;
        int minDistance = INT_MAX;

        for (const auto& elevator : elevators) {
            int distance = std::abs(elevator->getCurrentFloor() - request.floor);
            Direction dir = elevator->getCurrentDirection();

            // Calculate cost factor based on direction alignment
            int cost = distance;
            if (dir != Direction::IDLE && dir != request.direction) {
                cost += 10; // Penalty for moving in opposite direction
            }

            if (cost < minDistance) {
                minDistance = cost;
                bestElevator = elevator;
            }
        }

        return bestElevator;
    }
};