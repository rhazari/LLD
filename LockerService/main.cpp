#include "LockerModels.hpp"
#include "AllocationStrategy.hpp"
#include "LockerService.hpp"
#include <iostream>
#include <vector>
#include <memory>

int main() {
    std::vector<std::shared_ptr<Compartment>> compartments = {
        std::make_shared<Compartment>("C101", Size::SMALL),
        std::make_shared<Compartment>("C102", Size::MEDIUM),
        std::make_shared<Compartment>("C103", Size::LARGE)
    };

    LockerService locker("LOCKER_HUB_01", compartments, std::make_unique<BestFitAllocationStrategy>());

    auto smallPackage = std::make_shared<Package>("PKG_001", Size::SMALL);
    auto mediumPackage = std::make_shared<Package>("PKG_002", Size::MEDIUM);

    std::cout << "--- Delivery Phase ---\n";
    auto otp1 = locker.reserveSlot(smallPackage);
    auto otp2 = locker.reserveSlot(mediumPackage);

    if (otp1.has_value()) {
        locker.depositPackage(otp1.value());
    }

    std::cout << "\n--- Customer Pickup Phase ---\n";
    if (otp1.has_value()) {
        locker.pickupPackage(otp1.value());
    }

    std::cout << "\n--- Invalid OTP Attempt ---\n";
    locker.pickupPackage("000000");

    return 0;
}