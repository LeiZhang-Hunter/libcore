#include "component/thread_container.h"

#include "os/unix_logger.h"
#include "os/unix_countdown_latch.h"

namespace Core {
namespace Component {

void UnixThreadContainer::start() {
    std::shared_ptr<OS::UnixCountDownLatch> latch = std::make_shared<OS::UnixCountDownLatch>(size());
    for(auto iter = container.begin(); iter != container.end(); iter++) {
        iter->second->addInitCallable([latch] {
            latch->down();
        });
        bool ret = iter->second->start();
        if (!ret) {
            SYSTEM_ERROR_LOG_TRACE("create thread failed!");
        }
    }
    latch->wait();
}

void UnixThreadContainer::stop() {
    for(auto iter = container.begin(); iter != container.end(); iter++) {
        iter->second->stop();
    }
}

bool UnixThreadContainer::reg(pid_t index, const std::shared_ptr<OS::UnixThread>& thread) {
    auto iter = container.find(index);
    if (iter != container.end()) {
        SYSTEM_ERROR_LOG_TRACE("Thread has been registered!");
        return false;
    }

    container[index] = thread;
    return true;
}

bool UnixThreadContainer::task(pid_t index, const Event::Task& task) {
    auto iter = container.find(index);
    if (iter == container.end()) {
        SYSTEM_ERROR_LOG_TRACE("No thread index found!");
        return false;
    }

    iter->second->addTask(task);
    return true;
}

void UnixThreadContainer::broadcastEvent(const Event::Task& task) {
    for(auto iter = container.begin(); iter != container.end(); iter++) {
        iter->second->addTask(task);
    }
}

std::shared_ptr<OS::UnixThread>& UnixThreadContainer::getThread(int index) {
     auto iter = container.find(index);
     if (iter == container.end()) {
         return empty;
     }
     return iter->second;
}
}
}
