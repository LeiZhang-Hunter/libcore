#pragma once

#include "event/event_loop.h"
#include "non_copyable.h"

namespace Core {
namespace Http {

/**
 * 线程管理器
 */
class HttpWorker : public Core::Noncopyable {

public:
    HttpWorker(const std::string& ip_, uint16_t port_) :ip(ip_), port(port_) {};

    void init(const std::shared_ptr<Event::EventLoop>& loop);

    int openServer();

    void stop() {
        if (listener != -1) {
            evhttp_del_accept_socket(httpHandle.get(), bound);
            close(listener);
            listener = -1;
        }
    }

    ~HttpWorker();

    void setCallable(void (*cb)(struct evhttp_request *, void *), void* args_ = nullptr) {
        if (!cb) {
            return;
        }
        httpRequestHandle = cb;
        args = args_;
    }

    void setTimeout(int timeout) {
        if (timeout < 0) {
            return;
        }
        httpTimeout = timeout;
    }

private:

    /**
    * @brief http  的句柄
    *
    */
    Event::EventHttpPtr httpHandle;

    /**
     * 事件回调
     */
    void (*httpRequestHandle)(struct evhttp_request *, void *) = nullptr;

    int listener = -1;

    int httpTimeout = 60;

    void* args = nullptr;

    std::string ip;

    uint16_t port;

    struct evhttp_bound_socket* bound = nullptr;
};
}
}
