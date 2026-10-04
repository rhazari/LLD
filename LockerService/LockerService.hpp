#pragma once
#include "LockerModels.hpp"
#include "AllocationStrategy.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <mutex>
#include <random>

class LockerService {
private:
    std::string lockerId;
    std::vector<std::shared_ptr<Compartment>> compartments;
    std::unique_ptr<AllocationStrategy> allocationStrategy;
    
    std::unordered_map<std::string, std::shared_ptr<AccessToken>> activeTokens;
    std::unordered_map<std::string, std::shared_ptr<Compartment>> tokenToCompartmentMap;
    std::unordered_map<std::string, std::shared_ptr<Package>> tokenToPackageMap;
    
    std::mutex serviceMutex;

    std::string generateUniqueOtp() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100000, 999999);
        return std::to_string(dis(gen));
    }

public:
    LockerService(std::string id, 
                  std::vector<std::shared_ptr<Compartment>> comps, 
                  std::unique_ptr<AllocationStrategy> strategy)
        : lockerId(std::move(id)), 
          compartments(std::move(comps)), 
          allocationStrategy(std::move(strategy)) {}

    std::optional<std::string> reserveSlot(std::shared_ptr<Package> pkg) {
        std::lock_guard<std::mutex> lock(serviceMutex);

        auto targetCompartment = allocationStrategy->findCompartment(compartments, pkg->getSize());
        if (!targetCompartment) {
            std::cout << "[LockerService] No available compartment for package size!\n";
            return std::nullopt;
        }

        if (targetCompartment->reserve()) {
            std::string code = generateUniqueOtp();
            auto token = std::make_shared<AccessToken>(code, 72);

            activeTokens[code] = token;
            tokenToCompartmentMap[code] = targetCompartment;
            tokenToPackageMap[code] = pkg;

            std::cout << "[LockerService] Reserved Compartment " << targetCompartment->getCompartmentId()
                      << " | OTP Generated: " << code << "\n";
            return code;
        }
        return std::nullopt;
    }

    bool depositPackage(const std::string& otpCode) {
        std::lock_guard<std::mutex> lock(serviceMutex);

        auto tokenIt = activeTokens.find(otpCode);
        if (tokenIt == activeTokens.end() || !tokenIt->second->isValid()) {
            std::cout << "[LockerService] Deposit Failed: Invalid or expired OTP.\n";
            return false;
        }

        auto compartment = tokenToCompartmentMap[otpCode];
        auto pkg = tokenToPackageMap[otpCode];

        compartment->depositPackage(pkg);
        std::cout << "[LockerService] Package " << pkg->getPackageId() 
                  << " deposited in Compartment " << compartment->getCompartmentId() << "\n";
        return true;
    }

    std::shared_ptr<Package> pickupPackage(const std::string& otpCode) {
        std::lock_guard<std::mutex> lock(serviceMutex);

        auto tokenIt = activeTokens.find(otpCode);
        if (tokenIt == activeTokens.end() || !tokenIt->second->isValid()) {
            std::cout << "[LockerService] Pickup Failed: Invalid or expired OTP.\n";
            return nullptr;
        }

        auto compartment = tokenToCompartmentMap[otpCode];
        auto pkg = compartment->releasePackage();
        tokenIt->second->markUsed();

        activeTokens.erase(otpCode);
        tokenToCompartmentMap.erase(otpCode);
        tokenToPackageMap.erase(otpCode);

        std::cout << "[LockerService] Success! Package " << pkg->getPackageId() 
                  << " retrieved from Compartment " << compartment->getCompartmentId() << "\n";
        return pkg;
    }
};