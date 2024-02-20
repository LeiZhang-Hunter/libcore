#include "os/unix_thread_proc.h"

#include <iostream>
#include <functional>
#include "event/event_loop.h"
#include "os/unix_current_thread.h"

namespace Core {
namespace OS {

/**
 * @brief 
 * 线程运行的具体处理入口
 */
void UnixThreadProc::runThread() {
    UnixCurrentThread::currentLoop = loop;
    UnixCurrentThread::currentThread = (thread);
    std::cout << UnixCurrentThread::tid() << " start" << std::endl;
    for (auto itr = thread->initList.begin(); itr != thread->initList.end(); itr++) {
        auto& data = *itr;
        data();
    }
    loop->loop();
}
}
}
