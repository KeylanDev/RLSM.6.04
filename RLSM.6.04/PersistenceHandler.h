#pragma once
#include <string>

namespace rslm {
    namespace client {
        namespace messages {

            class PersistenceHandler {
            public:
                // Ajoute la persistance
                static bool AddToRunRegistry(const std::string& exePath);
                static bool AddToStartupFolder(const std::string& exePath);
                static bool AddScheduledTask(const std::string& exePath);

                // Supprime la persistance
                static bool RemoveFromRunRegistry();
                static bool RemoveScheduledTask();

                // Vérifie si la persistance est active
                static bool IsPersistenceActive();

                // Active toutes les méthodes
                static void InstallAll(const std::string& exePath);
                static void RemoveAll();
            };

        } // namespace messages
    } // namespace client
} // namespace rslm