#pragma once
#include "Message.h"
#include <functional>
#include <string>

namespace rslm {
	namespace client {
		namespace agent {

			using SendFn = std::function<void(const net::Message&)>;

			class AgentMessageDispatcher {
			public:
				static void Configure(const std::string& agentId, SendFn send, SendFn push);
				static void Dispatch(const net::Message& request);

			private:
				static void Reply(const net::Message& request, const net::json& payload);
				static void ReplyError(const net::Message& request, const std::string& error);
			};

		} // namespace agent
	} // namespace client
} // namespace rslm
