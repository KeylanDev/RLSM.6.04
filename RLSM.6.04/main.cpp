#include <iostream>
#include <thread>
#include <chrono>
#include "RSLMServer.h"

int main() {
    std::cout << "[RSLM Server] Starting on port 4782...\n";

    rslm::server::networking::RSLMServer server;

    if (server.Start(4782)) {
        std::cout << "[RSLM Server] Server is running!\n";

        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    else {
        std::cerr << "[RSLM Server] FAILED to start!\n";
        return 1;
    }

    return 0;
}