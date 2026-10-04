#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <chrono>

enum class Size { SMALL = 1, MEDIUM = 2, LARGE = 3, EXTRA_LARGE = 4 };

inline bool fitsIn(Size packageSize, Size compartmentSize) {
    return static_cast<int>(packageSize) <= static_cast<int>(compartmentSize);
}

enum class CompartmentState { AVAILABLE, RESERVED, OCCUPIED, UNDER_MAINTENANCE };

class Package {
private:
    std::string packageId;
    Size size;
public:
    Package(std::string id, Size s) : packageId(std::move(id)), size(s) {}
    std::string getPackageId() const { return packageId; }
    Size getSize() const { return size; }
};

class AccessToken {
private:
    std::string code;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point expiresAt;
    bool used;
public:
    AccessToken(std::string otp, int validityHours)
        : code(std::move(otp)),
          createdAt(std::chrono::system_clock::now()),
          expiresAt(createdAt + std::chrono::hours(validityHours)),
          used(false) {}
    bool isValid() const { return !used && (std::chrono::system_clock::now() < expiresAt); }
    void markUsed() { used = true; }
    std::string getCode() const { return code; }
};

class Compartment {
private:
    std::string compartmentId;
    Size size;
    CompartmentState state;
    std::shared_ptr<Package> currentPackage;
    mutable std::mutex compMutex;
public:
    Compartment(std::string id, Size s)
        : compartmentId(std::move(id)), size(s), state(CompartmentState::AVAILABLE) {}

    bool reserve() {
        std::lock_guard<std::mutex> lock(compMutex);
        if (state == CompartmentState::AVAILABLE) {
            state = CompartmentState::RESERVED;
            return true;
        }
        return false;
    }

    void depositPackage(std::shared_ptr<Package> pkg) {
        std::lock_guard<std::mutex> lock(compMutex);
        currentPackage = std::move(pkg);
        state = CompartmentState::OCCUPIED;
    }

    std::shared_ptr<Package> releasePackage() {
        std::lock_guard<std::mutex> lock(compMutex);
        auto pkg = currentPackage;
        currentPackage = nullptr;
        state = CompartmentState::AVAILABLE;
        return pkg;
    }

    std::string getCompartmentId() const { return compartmentId; }
    Size getSize() const { return size; }
    CompartmentState getState() const {
        std::lock_guard<std::mutex> lock(compMutex);
        return state;
    }
};