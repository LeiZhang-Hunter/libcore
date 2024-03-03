#pragma once

#include <string>
#include <utility>
#include <memory>
#include <map>
#include <vector>
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
    Status status_ = SUCCESS;
    std::string message_;
};

class EventData {
public:
    virtual std::map<std::string, std::string> meta() PURE;
    virtual std::map<std::string, std::string> header() PURE;
    virtual void* data() PURE;
    virtual std::string name() PURE;
    // Fill event with meta,header,body cannot be nil
    virtual void  fill(const std::map<std::string, std::string>& meta, const std::map<std::string, std::string>& header, void* data) PURE;
    virtual void release() PURE;
};

class Batch :public Nonmoveable, Noncopyable {
public:
    virtual std::map<std::string, std::string> meta() PURE;
    virtual std::vector<EventData>& events() PURE;
    virtual void Release() PURE;
};

class Queue :public Nonmoveable, Noncopyable{
public:
    virtual void product(std::shared_ptr<EventData>& e) PURE;
};


class Consumer :public Noncopyable, Nonmoveable {
public:
    virtual Result Consume(std::shared_ptr<Batch>& batch) PURE;

    virtual std::shared_ptr<Queue>& channel() PURE;

    virtual bool bindChannel(std::shared_ptr<Queue> channel) PURE;

    virtual Result Consume(Batch& batch) PURE;
};

}