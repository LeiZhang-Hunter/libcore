#pragma once

extern "C" {
#include <event.h>
#include <unistd.h>
}
#include <iostream>
#include <memory>
namespace Core {
namespace Event {

class EventLoop;

class EventChannel : public std::enable_shared_from_this<EventChannel> {
public:

    EventChannel(const std::shared_ptr<EventLoop> &loop_, int fd_, bool auto_close = true) : fd(fd_),
                                                                                             loop(loop_),
                                                                                             auto_close_(
                                                                                                     auto_close) {}

    virtual bool eventSet(const std::shared_ptr<EventLoop> &loop) {
        return true;
    }

    /**
     * @brief 获取channel的fd
     *
     * @return int
     */
    int getChannelFd() {
        return fd;
    }

    void close();

    void shutdownWrite();

    void shutdownRead();

    unsigned char getEvents() const {
        return events;
    }

    virtual bool enable(double second) {
        return true;
    };

    virtual bool enable(unsigned char events_, __time_t second) {
        return true;
    }

    virtual bool disableReading() {
        return true;
    };

    virtual bool disableWriting() {
        return true;
    }

    virtual bool enableWriting(double second) {
        return true;
    }

    virtual bool enableReading(double second) {
        return true;
    }

    /**
     * @brief Set the Events object
     *
     * @param _events
     * @return true
     * @return false
     */
    bool setEvents(uint32_t _events) {
        return events = _events;
    }

    /**
     * @brief Get the Timer object
     *
     * @return struct timeval&
     */
    struct timeval &getTimer() {
        return time;
    }

    virtual ~EventChannel() {
        std::cout << "~EventChannel:" << fd << std::endl;
        if (fd >= 0 && auto_close_) {
            ::close(fd);
            fd = -1;
        }
    }

protected:

    void tie(const std::shared_ptr<void> &obj) {
        tie_ = obj;
    }

    bool auto_close_ = true;

    /**
    * @brief 事件循环
    *
    */
    const std::shared_ptr<EventLoop> &loop;

    /**
     * 这是为了保护channel 存活的
     */
    std::weak_ptr<void> tie_;

    //管道的超时时间
    struct timeval time;

    /**
     * @brief 事件标志位
     *
     */
    unsigned char events = EV_PERSIST;

private:
    /**
     * @brief 描述符
     *
     */
    int fd = -1;

    int closeType = SHUT_RDWR;
};

}
}