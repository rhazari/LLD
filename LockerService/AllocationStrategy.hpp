#pragma once
#include "LockerModels.hpp"
#include <vector>
#include <memory>

class AllocationStrategy {
public:
    virtual ~AllocationStrategy() = default;
    virtual std::shared_ptr<Compartment> findCompartment(
        const std::vector<std::shared_ptr<Compartment>>& compartments, 
        Size packageSize) = 0;
};

class BestFitAllocationStrategy : public AllocationStrategy {
public:
    std::shared_ptr<Compartment> findCompartment(
        const std::vector<std::shared_ptr<Compartment>>& compartments, 
        Size packageSize) override 
    {
        std::shared_ptr<Compartment> bestMatch = nullptr;
        for (const auto& comp : compartments) {
            if (comp->getState() == CompartmentState::AVAILABLE && fitsIn(packageSize, comp->getSize())) {
                if (!bestMatch || static_cast<int>(comp->getSize()) < static_cast<int>(bestMatch->getSize())) {
                    bestMatch = comp;
                }
            }
        }
        return bestMatch;
    }
};