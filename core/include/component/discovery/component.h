/**
******************************************************************************
* @file           : component.cc
* @author         : zhanglei
* @brief          : None
* @attention      : None
* @date           : 2024/2/11
******************************************************************************
*/
//
// Created by zhanglei on 2024/2/11.
//

#pragma once
#include <utility>

#include "watcher.h"
#include "os/unix_logger.h"
#include "component/api.h"
#include "event/event_channel.h"
#include "event/event_buffer_channel.h"

namespace Core {
namespace Component {
namespace Discovery {
class Component :public Core::Component::Component {
public:
    explicit Component(const std::string& configPath) :watcher(std::make_unique<Watcher>()) {
        if (!watcher->reg(configPath, IN_CREATE | IN_MODIFY | IN_DELETE)) {
            SYSTEM_ERROR_LOG_TRACE("watcher->reg failed({}):{}", errno, strerror(errno));
            return;
        }
    }

    ~Component() {

    }

    void init() {

    };

    void start() {

    };

    void stop() {
        watcher = nullptr;
    };

    void finish() {

    };

    std::string name() override {
        return "discovery";
    }

    void setListener(std::shared_ptr<ConfigListener> listener_) {
        listener = std::move(listener_);
    }

    std::shared_ptr<Event::EventChannel> channel(const std::shared_ptr<Event::EventLoop>& loop) {
        auto channel = std::make_shared<Event::EventBufferChannel>(loop, watcher->getFd());
        channel->bindOnReadCallable([ptr = this](auto && bev, auto && channel){
            struct inotify_event event;
            read(ptr->watcher->getFd(), &event, sizeof(event));
            if (event.mask & IN_CREATE) {
                if (ptr->listener)
                    ptr->listener->onCreate();
            }
            if (event.mask & IN_MODIFY) {
                if (ptr->listener)
                    ptr->listener->onUpdate();
            }
            if (event.mask & IN_DELETE) {
                if (ptr->listener)
                    ptr->listener->onDelete();
            }
        }, -1);
        return channel;
    }

private:
    std::unique_ptr<Watcher> watcher;
    std::shared_ptr<ConfigListener> listener;
};
}
}
}
