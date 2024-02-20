//
// Created by zhanglei on 2/14/22.
//

#ifndef CORE_EVENT_EVENT_TIMER_H
#define CORE_EVENT_EVENT_TIMER_H

#include <memory>
#include <functional>
#include "event_smart_ptr.h"

namespace Core {
namespace Event {

class EventLoop;

class EventTimerHelper;

class EventTimer {
public:
    explicit EventTimer(const std::shared_ptr<EventLoop> &loop);

    void disable();

    bool enable(const std::function<void()> &callable_, time_t millisecond);

    ~EventTimer();

private:
    friend class EventTimerHelper;

    EventPtr ptr;
    std::function<void()> callable;
    EventLoop *loop_;
    EventTimerHelper *helper = nullptr;
};
}
}
#endif //CORE_EVENT_EVENT_TIMER_H
