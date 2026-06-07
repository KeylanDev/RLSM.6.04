#include "DateTimeHelper.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace rslm {
    namespace client {
        namespace helper {

            int64_t DateTimeHelper::Now() {
                auto now = std::chrono::system_clock::now();
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()
                ).count();
            }

            std::string DateTimeHelper::ToString(int64_t timestamp) {
                auto tp = std::chrono::system_clock::time_point(
                    std::chrono::milliseconds(timestamp)
                );
                std::time_t tt = std::chrono::system_clock::to_time_t(tp);

                std::tm tm;
#ifdef _WIN32
                localtime_s(&tm, &tt);
#else
                localtime_r(&tt, &tm);
#endif

                std::ostringstream oss;
                oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
                return oss.str();
            }

            int64_t DateTimeHelper::FromString(const std::string& dateStr) {
                std::tm tm = {};
                std::istringstream iss(dateStr);
                iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

                if (iss.fail()) {
                    return 0;
                }

                std::time_t tt = std::mktime(&tm);
                return static_cast<int64_t>(tt) * 1000;
            }

            std::string DateTimeHelper::GetCurrentFormatted() {
                return ToString(Now());
            }

        } // namespace helper
    } // namespace client
} // namespace rslm