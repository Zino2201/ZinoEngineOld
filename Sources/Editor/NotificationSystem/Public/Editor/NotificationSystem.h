#pragma once

#include "EngineCore.h"

namespace ze::editor
{

/** API for managing notifications */

enum class NotificationType : uint8_t
{
    Info,
    Warning,
    Error
};

struct Notification
{
    NotificationType type;
    std::chrono::system_clock::time_point time;
    float duration;
    std::string message;
    float lifetime;

    Notification(NotificationType in_type, const std::chrono::system_clock::time_point in_time,
        const float in_duration, const std::string& in_message) :
        type(in_type), time(in_time), duration(in_duration), message(in_message), lifetime(0.f) {}
};

/**
 * Add a new notification
 */
NOTIFICATIONSYSTEM_API void notification_add(NotificationType in_type, const std::string& in_message, const float in_duration = 5.0f);
NOTIFICATIONSYSTEM_API void notifications_update(const float& in_delta_time);
NOTIFICATIONSYSTEM_API const std::vector<Notification>& notifications_get();

}
