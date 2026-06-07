#include "Message.h"
#include <stdexcept>

namespace rslm {
    namespace net {

        Message::Message(const nlohmann::json& j) {
            type = j.value("type", "");
            senderId = j.value("senderId", "");
            targetId = j.value("targetId", "");
            tag = j.value("tag", 0u);
            timestamp = j.value("timestamp", 0ll);
            payload = j.value("payload", nlohmann::json::object());
        }

        nlohmann::json Message::ToJson() const {
            return {
                {"type", type},
                {"senderId", senderId},
                {"targetId", targetId},
                {"tag", tag},
                {"timestamp", timestamp},
                {"payload", payload}
            };
        }

        std::string Message::ToJsonString() const {
            return ToJson().dump();
        }

        Message Message::FromJson(const std::string& jsonStr) {
            return Message(nlohmann::json::parse(jsonStr));
        }

        Message Message::CreateRequest(const std::string& type,
            const std::string& sender,
            const std::string& target,
            const nlohmann::json& data) {
            Message msg;
            msg.type = type;
            msg.senderId = sender;
            msg.targetId = target;
            msg.payload = data;
            return msg;
        }

        Message Message::CreateResponse(const Message& request, const nlohmann::json& data) {
            Message msg;
            msg.type = request.type + "-response";
            msg.senderId = request.targetId;
            msg.targetId = request.senderId;
            msg.tag = request.tag;
            msg.payload = data;
            return msg;
        }

        Message Message::CreateError(const Message& request, const std::string& error) {
            Message msg;
            msg.type = request.type + "-error";
            msg.senderId = request.targetId;
            msg.targetId = request.senderId;
            msg.tag = request.tag;
            msg.payload = { {"error", error} };
            return msg;
        }

    } // namespace net
} // namespace rslm