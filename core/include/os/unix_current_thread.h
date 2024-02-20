#pragma once

extern "C" {
#include <unistd.h>
#include <sys/syscall.h>
}

#include <memory>
#include <thread>
#include "build_expect.h"
#include "os/unix_thread.h"
#include "event/event_loop.h"

namespace Core {
namespace OS {

namespace UnixCurrentThread {

extern thread_local int t_cachedTid;

extern thread_local std::shared_ptr<Core::Event::EventLoop> currentLoop;

extern thread_local std::shared_ptr<UnixThread> currentThread;


inline int tid() {
    if (build_unlikely((t_cachedTid == 0))) {
        t_cachedTid = syscall(SYS_gettid);
    }
    return t_cachedTid;
}

inline Event::EventLoop *loop() {
    return currentLoop.get();
}

inline std::shared_ptr<Event::EventLoop> &loopSmartPtr() {
    return currentLoop;
}

void setCurrentThread(std::shared_ptr<UnixThread> &thread_);


inline std::shared_ptr<UnixThread> &getThread() {
    return currentThread;
}
}
}
}