#pragma once

#include <string>
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
}