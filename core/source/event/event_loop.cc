#include "os/unix_logger.h"
#include "event/event_loop.h"
#include "os/unix_logger.h"
#include "event/event_channel.h"

namespace Core {
namespace Event {

void EventLoop::loop() {
    if (!base) {
        return;
    }
    //循环、分发事件
    int ret = event_base_dispatch(base.get());
    if (ret != 0) {
        std::string error = ("create event loop error;error ret:" + std::to_string(ret));
        SYSTEM_ALERT_LOG_TRACE(error)
        LOG_CONTENT_ERR(error);
        exit(-1);
    }
}

void EventLoop::quit() {
    if (!base) {
        SYSTEM_ALERT_LOG_TRACE("base error!")
        throw std::runtime_error("base error!");
        return;
    }

    //循环、分发事件
    int res = event_base_loopexit(base.get(), nullptr);
    if (res != 0) {
        SYSTEM_WARNING_LOG_TRACE("event_base_loopexit failed");
    }
}

//更新管道
bool EventLoop::updateChannel(const std::shared_ptr<EventChannel> &channel) {
    //这里不要加锁，不要发生竞争关系
    int fd = channel->getChannelFd();
    auto it = channels.find(fd);
    if (it == channels.end()) {
        channels.insert({fd, channel});
    } else {
        channels[fd] = channel;
    }
    channel->eventSet(shared_from_this());
    return true;
}

bool EventLoop::deleteChannel(const std::shared_ptr<EventChannel> &channel) {
    //这里不要加锁，不要发生竞争关系
    int fd = channel->getChannelFd();
    if (fd == -1) {
        SYSTEM_WARNING_LOG_TRACE("channel fd has closed");
        return false;
    }
    auto it = channels.find(fd);

    if (it != channels.end()) {
        channels.erase(it);
        return true;
    }
    return false;
}

bool EventLoop::deleteChannelByFd(int fd) {
    //这里不要加锁，不要发生竞争关系
    auto it = channels.find(fd);

    if (it != channels.end()) {
        channels.erase(it);
        return true;
    }
    return false;
}



// bool EventLoop::eventNoBufferSet(const std::shared_ptr<EventChannel>& channel) {
//     //
//     //添加新事件
//     struct event *ev_sockfd = event_new(base.get(), channel->getChannelFd(), channel->getEvents(),
//         onNoBufferEvent, static_cast<void *>(channel.get()));

//     struct timeval time;
//     time.tv_sec = channel->getTimer().tv_sec;
//     time.tv_usec = channel->getTimer().tv_usec;
//     event_add(ev_sockfd, &time);

//     return true;
// }
}
}