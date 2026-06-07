#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include "TcpClient.h"
#include "Message.h"
#include "MessageFramer.h"
#include "Settings.h"
#include "AgentMessageDispatcher.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <gdiplus.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "gdiplus.lib")

using namespace rslm;

static std::atomic<bool> g_running{ true };
static std::mutex g_sendMutex;
static client::networking::TcpClient* g_clientPtr = nullptr;

static std::string GetLocalIpAddress() {
    std::string ip = "127.0.0.1";
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            struct hostent* host = gethostbyname(hostname);
            if (host && host->h_addr_list[0]) {
                ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
            }
        }
        WSACleanup();
    }
    return ip;
}

static void SendWireMessage(client::networking::TcpClient& client, const net::Message& msg) {
    std::lock_guard<std::mutex> lock(g_sendMutex);
    std::string wire = msg.ToJsonString() + "\n";
    client.Send(std::vector<uint8_t>(wire.begin(), wire.end()));
}

int main() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    client::config::Settings settings;

    std::cout << "[RSLM Agent] Connecting to "
              << settings.serverHost << ":" << settings.serverPort << "...\n";

    client::networking::TcpClient client;
    net::MessageFramer framer;
    std::string agentId;

    client::agent::AgentMessageDispatcher::Configure(
        agentId,
        [&](const net::Message& msg) { SendWireMessage(client, msg); },
        [&](const net::Message& msg) { SendWireMessage(client, msg); });

    g_clientPtr = &client;

    while (g_running) {
        if (!client.Connect(settings.serverHost, settings.serverPort)) {
            std::cerr << "[RSLM Agent] Connection failed, retry in "
                      << settings.reconnectDelay << "s\n";
            std::this_thread::sleep_for(std::chrono::seconds(settings.reconnectDelay));
            continue;
        }

        std::cout << "[RSLM Agent] Connected.\n";

        std::atomic<bool> connected{ true };

        client.StartReceiving(
            [&](const std::vector<uint8_t>& data) {
                framer.Append(reinterpret_cast<const char*>(data.data()), data.size(),
                    [&](const std::string& line) {
                        try {
                            auto msg = net::Message::FromJson(line);

                            if (msg.type == net::MessageType::HELLO_ACK) {
                                agentId = msg.payload.value("clientId", std::string());
                                client::agent::AgentMessageDispatcher::Configure(
                                    agentId,
                                    [&](const net::Message& reply) { SendWireMessage(client, reply); },
                                    [&](const net::Message& push) { SendWireMessage(client, push); });
                                std::cout << "[RSLM Agent] Registered as " << agentId << "\n";
                                return;
                            }

                            if (agentId.empty())
                                return;

                            client::agent::AgentMessageDispatcher::Dispatch(msg);
                        }
                        catch (const std::exception& ex) {
                            std::cerr << "[RSLM Agent] Parse error: " << ex.what() << "\n";
                        }
                    });
            },
            [&]() { connected = false; });

        std::string localIp = GetLocalIpAddress();
        net::Message hello = net::Message::CreateRequest(
            net::MessageType::HELLO, "", "server",
            { {"role", "agent"}, {"tag", localIp} });
        SendWireMessage(client, hello);

        while (connected && client.IsConnected()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        std::cout << "[RSLM Agent] Disconnected, reconnecting...\n";
        client.Disconnect();
        framer.Clear();
        agentId.clear();
        std::this_thread::sleep_for(std::chrono::seconds(settings.reconnectDelay));
    }

    return 0;
}
