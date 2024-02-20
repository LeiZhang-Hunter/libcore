#pragma once

#include <cctype>
#include <iostream>
#include <memory>
#include <map>
#include <unordered_map>

#include "build_expect.h"
#include "os/unix_logger.h"
#include "os/unix_sigset.h"
#include "event/event_smart_ptr.h"
#include "event/event_channel.h"

/**
* 信号派遣的函数类型
*/
typedef void(*dispatcherSignal)(evutil_socket_t sig, short events, void *param);

namespace Core {
namespace Event {


//管道的map
typedef std::unordered_map<int, std::shared_ptr<EventChannel>> ChannelMap;

typedef std::unordered_map<int, EventPtr> SignalSlot;

/**
* @brief
*
*/
class EventLoop : public std::enable_shared_from_this<EventLoop> {
public:
    EventLoop() : base(event_init()) {}

    /**
     * 获取基址
     * @return
     */
    struct event_base *getEventBase() {
        return base.get();
    }

    /**
     * @brief
     * 事件回调
     * @param ev
     * @param flag
     * @param arg
     */
    static void onEvent(bufferevent *ev, short flag, void *arg);


    /**
     * @brief 不带缓冲区的事件回调
     *
     */
    static void onNoBufferEvent(int fd, short events, void *arg);

    //添加信号
    bool sigAdd(int sigNo, dispatcherSignal handle, void *userData) {
        EventPtr signale = evsignal_new(base.get(), sigNo, handle, userData);
        if (event_add(signale.get(), nullptr)) {
            SYSTEM_ERROR_LOG_TRACE("sig add error");
            return false;
        }
        slot[sigNo] = std::move(signale);
        return true;
    }

    //退出
    void quit();

    //事件循环
    void loop();

    /**
     * 更新管道
     * @param channel
     * @return
     */
    bool updateChannel(const std::shared_ptr<EventChannel> &channel);

    /**
     * 删除管道
     * @param channel
     * @return
     */
    bool deleteChannel(const std::shared_ptr<EventChannel> &channel);

    /**
     * @brief 删除管道通过描述符
     *
     * @param fd
     * @return true
     * @return false
     */
    bool deleteChannelByFd(int fd);


    ~EventLoop() {
#ifdef USE_DEBUG
        std::cout << "~EventLoop" << std::endl;
#endif
    }

private:
    /**
     * 设置事件
     * @param operation
     * @param channel
     * @return
     */
    bool eventSet(const std::shared_ptr<EventChannel> &channel);

    /**
    * 设置不带缓冲区的事件回调
    * @param operation
    * @param channel
    * @return
    */
    bool eventNoBufferSet(const std::shared_ptr<EventChannel> &channel);

    //libevent的基址
    // struct event_base *base = nullptr;
    Event::EventBasePtr base;

    /**
     * 管道集合
    */
    ChannelMap channels = {};

    OS::UnixSigSet set;

    /**
     * 信号槽，放在这里是为了防止 信号对象被析构
     */
    SignalSlot slot = {};
};
} // namespace Event
}