#include "ElevatorModels.hpp"
#include "Elevator.hpp"
#include "ElevatorDispatcher.hpp"
#include "ElevatorController.hpp"

int main() {
    // Initialize system with 2 Elevators and a Nearest-Elevator strategy
    ElevatorController controller(2, std::make_unique<NearestElevatorStrategy>());

    std::cout << "--- Elevators Initialized at Floor 1 ---\n";

    // 1. External call: Someone at Floor 5 wants to go UP
    controller.handleExternalRequest(5, Direction::UP);

    // Give time for simulation loop to process
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 2. Someone inside Elevator 1 selects Floor 8
    controller.handleInternalRequest(1, 8);

    // 3. External call: Someone at Floor 3 wants to go DOWN
    controller.handleExternalRequest(3, Direction::DOWN);

    // Allow time for elevators to complete movements
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Shutdown worker threads safely
    controller.shutdown();
    std::cout << "\n--- System Shutdown Cleanly ---\n";

    return 0;
}