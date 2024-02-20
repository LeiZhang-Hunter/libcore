#pragma once
#include "pure.h"

namespace Core {
namespace Component {
namespace Process {
class Manager;
class ProcessEvent {
    virtual void onCreate() PURE;
    virtual void onStart() PURE;
    virtual void onStop() PURE;
    virtual void onDestroy() PURE;
    virtual void onReload() PURE;
    virtual void onExit(Manager* m) PURE;
};
}
}
}