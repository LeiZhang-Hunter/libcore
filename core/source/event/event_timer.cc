#include "event/event_timer.h"

extern "C" {
#include "event2/event.h"
}
#include "event/event_loop.h"
#include "os/unix_logger.h"

namespace Core {
namespace Event {

class EventTimerHelper {
public:
    explicit EventTimerHelper(EventTimer *timer_) {
        timer = timer_;
    }

    void doCallable() {
        timer->callable();
    }

    ~EventTimerHelper() {

    }

private:
    EventTimer *timer;
};

static void onTimerCallable(evutil_socket_t fd, short what, void *arg) {
    auto helper = (EventTimerHelper *) arg;
    helper->doCallable();
}

bool EventTimer::enable(const std::function<void()> &callable_, time_t millisecond) {
    callable = callable_;
    if (ptr) {
        evtimer_del(ptr.get());
    }
    ptr = evtimer_new(loop_->getEventBase(), onTimerCallable, helper);

    struct timeval ms = {.tv_sec= (millisecond / 1000), .tv_usec = (millisecond % 1000)};
    int ret = evtimer_add(ptr.get(), &ms);
    if (build_likely(ret == 0)) {
        return true;
    }
    LOG_CONTENT_ERR("evtimer_add error")
    return false;
}

void EventTimer::disable() {
    if (ptr)
        evtimer_del(ptr.get());
}

EventTimer::EventTimer(const std::shared_ptr<EventLoop> &loop) {
    helper = new EventTimerHelper(this);
    loop_ = loop.get();
}

EventTimer::~EventTimer() {
    if (helper) {
        delete helper;
    }
}
}
}