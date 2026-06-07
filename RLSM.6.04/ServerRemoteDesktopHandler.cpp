#include "ServerRemoteDesktopHandler.h"

namespace rslm {
    namespace server {
        namespace messages {

            void ServerRemoteDesktopHandler::StartStream(const std::string& clientId,
                int quality, int fps,
                FrameCallback onFrame) {
            }

            void ServerRemoteDesktopHandler::StopStream(const std::string& clientId) {
            }

            void ServerRemoteDesktopHandler::SendMouseEvent(const std::string& clientId,
                int x, int y,
                MouseAction action,
                int wheelDelta) {
            }

            void ServerRemoteDesktopHandler::SendKeyEvent(const std::string& clientId,
                int keyCode, bool keyDown) {
            }

        } // namespace messages
    } // namespace server
} // namespace rslm