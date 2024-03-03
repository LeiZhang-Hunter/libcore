#pragma once

#include <string>
#include <utility>
#include <map>
#include "non_copyable.h"
#include "non_moveable.h"
#include "pure.h"

namespace Core::Component {
class Lifecycle {
public:
    virtual void init() PURE;

    virtual void start() PURE;

    virtual void stop() PURE;

    virtual void finish() PURE;
};

class Component :public Lifecycle {
public:
    virtual std::string name() PURE;
};

class ConfigListener {
public:
    virtual void onUpdate() PURE;
    virtual void onCreate() PURE;
    virtual void onDelete() PURE;
};

class BaseConfig :public ConfigListener {

};

typedef enum {
    FAIL,
    SUCCESS,
    TIMEOUT,
    DROP
} Status;


class Result {
public:
    Status status() {
        return status_;
    }
    void changeStatusTo(Status status) {
        status_ = status;
    }
    std::string error() {
        return message_;
    }

    void setErrorInfo(std::string message) {
        message_ = std::move(message);
    }
private:
    Status status_;
    std::string message_;
};

class Event {
public:
    virtual std::map<std::string, std::string> meta() PURE;
    virtual std::map<std::string, std::string> header() PURE;
    virtual void* data() PURE;
    // Fill event with meta,header,body cannot be nil
    virtual void  fill(const std::map<std::string, std::string>& meta, const std::map<std::string, std::string>& header, void* data) PURE;
    virtual void release() PURE;
};

class Batch :public Nonmoveable, Noncopyable {
public:
    virtual std::map<std::string, std::string> meta() PURE;
    virtual std::vector<Event> events() PURE;
    virtual void Release() PURE;
};

class Channel :public Nonmoveable, Noncopyable{
public:
    virtual void product(Event e) PURE;
};


class Consumer :public Noncopyable, Nonmoveable {
public:
    virtual Result Consume(Batch batch) PURE;

    virtual std::unique_ptr<Channel>& channel() PURE;

    virtual bool bindChannel(std::unique_ptr<Channel> channel) PURE;
};

}