#pragma once
extern "C" {
#include <event2/event.h>
#include <event2/bufferevent.h>
}
#include <memory>
#include "event_queue.h"

namespace Core {
namespace OS {
class threadObject;
};
namespace Event {
class EventChannel;

class EventQueueHandler {
public:

    EventQueueHandler(const std::shared_ptr<OS::UnixThread> &threadObject) : queue(
            std::make_unique<EventQueue>(threadObject)) {};

    /**
     * @brief 投递任务
     *
     * @param task
     */
    bool pushTask(const Task &task) {
        return queue->pushTask(task);
    }

    /**
     * @brief 派遣任务处理
     *
     */
    void dispatchTask(struct bufferevent *bev, Event::EventChannel *channel);

private:
    std::unique_ptr<EventQueue> queue;
};
}
}