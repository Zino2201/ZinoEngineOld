#include "Editor/NotificationSystem.h"

namespace ze::editor
{

std::vector<Notification> notifications;

void notification_add(NotificationType in_type, const std::string& in_message, const float in_duration)
{
    notifications.emplace_back(in_type, std::chrono::system_clock::now(), in_duration, in_message);
}

void notifications_update(const float& in_delta_time)
{
    std::vector<size_t> notifications_to_remove;
    notifications_to_remove.reserve(5);

    for(size_t i = 0; i < notifications.size(); ++i)
    {
        auto& notification = notifications[i];
        notification.lifetime += in_delta_time;
        if(notification.lifetime >= notification.duration)
            notifications_to_remove.emplace_back(i);
    }

    std::sort(notifications_to_remove.begin(), notifications_to_remove.end(),
              [](const size_t& left, const size_t& right)
              {
                  return left > right;
              });
    for(const auto& idx : notifications_to_remove)
        notifications.erase(notifications.begin() + idx);
}

const std::vector<Notification>& notifications_get()
{
    return notifications;
}

}