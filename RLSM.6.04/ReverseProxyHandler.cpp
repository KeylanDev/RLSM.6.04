#include "ReverseProxyHandler.h"

namespace rslm {
    namespace client {
        namespace messages {

            static bool g_proxyRunning = false;

            void ReverseProxyHandler::Start(const std::string& targetHost, uint16_t targetPort,
                ProxyStatusCallback callback) {
                g_proxyRunning = true;
                if (callback) callback(true, "");
            }

            void ReverseProxyHandler::Stop() {
                g_proxyRunning = false;
            }

            void ReverseProxyHandler::SendData(const std::vector<uint8_t>& data) {
            }

            void ReverseProxyHandler::SetOnDataReceived(ProxyDataCallback callback) {
            }

            bool ReverseProxyHandler::IsRunning() {
                return g_proxyRunning;
            }

        } // namespace messages
    } // namespace client
} // namespace rslm