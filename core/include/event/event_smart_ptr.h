#pragma once

extern "C" {
#include <event.h>
#include <evhttp.h>
#include <event2/http.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
}
#include <memory>

namespace Core {
namespace Event {

/**
* @brief 重写unique_ptr为了更好的管理libevent指针
*
* @tparam T
* @tparam (*deleter)(T*)
*/
template<class T, void (*deleter)(T *)>
class EventSmartPtr : public std::unique_ptr<T, void (*)(T *)> {
public:
    EventSmartPtr() : std::unique_ptr<T, void (*)(T *)>(nullptr, deleter) {};

    EventSmartPtr(T *object) : std::unique_ptr<T, void (*)(T *)>(object, deleter) {}
};

/**
* @brief 定义libevent的智能指针类型
*
*/
typedef EventSmartPtr<event_base, event_base_free> EventBasePtr;
typedef EventSmartPtr<evbuffer, evbuffer_free> EventBufferPtr;
typedef EventSmartPtr<evhttp, evhttp_free> EventHttpPtr;
typedef EventSmartPtr<event, event_free> EventPtr;
typedef EventSmartPtr<::bufferevent, bufferevent_free> EventBufferEventPtr;
typedef EventSmartPtr<evconnlistener, evconnlistener_free> EventListenPtr;
typedef EventSmartPtr<evhttp_uri, evhttp_uri_free> EventHttpUriPtr;
typedef EventSmartPtr<evhttp_connection, evhttp_connection_free> EventConnPtr;
typedef EventSmartPtr<evhttp_connection, evhttp_connection_free> EventConnPtr;
}
}