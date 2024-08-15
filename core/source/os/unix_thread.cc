#include "os/unix_thread.h"

#include "build_expect.h"
#include "os/unix_logger.h"
#include "os/unix_thread_proc.h"
#include "event/event_loop.h"
#include "event/event_buffer_channel.h"
#include "os/unix_current_thread.h"

namespace Core {
namespace OS {

UnixThread::UnixThread() :
loop((std::make_shared<Event::EventLoop>())),
daemonize(false),
mTerminated(false),
isSuspend(false),
mRunStatus(false),
wakeupChannelFd(createChannelFd()) {}

int UnixThread::ThreadProc(std::unique_ptr<UnixThreadProc> proc, const std::shared_ptr<UnixThread>& object) {
    if (!object->getName().empty()) {
        OS::set_thread_name(object->getName().c_str());
    }
    object->setTid(syscall(SYS_gettid));
    proc->runThread();
    object->destroy();
    return 0;
}

void UnixThread::stop() {
    queue->pushTask(std::bind(&Event::EventLoop::quit, loop));
    if ((mRunStatus) && ((!daemonize))) {
        join();
        mRunStatus = false;
    }
}

ssize_t UnixThread::wakeUp() {
    uint64_t notify = 1;
    ssize_t n = ::write(wakeupChannelFd, &notify, sizeof(notify));
    if (n != sizeof(notify)) {
        std::string error("thread ");
        error += std::to_string(getTid()) + " wake up " + std::to_string(wakeupChannelFd) + " failed !";
        SYSTEM_ALERT_LOG_TRACE(error)
        std::cerr << error << std::endl;
    }
    return n;
}

bool UnixThread::start() {
    // //构建队列
    queue = std::make_shared<Event::EventQueueHandler>(shared_from_this());

    if (build_unlikely(wakeupChannelFd == -1)) {
        SYSTEM_ALERT_LOG_TRACE("wakeupChannelFd create failed!");
        exit(-1);
    }

    std::unique_ptr<UnixThreadProc> proc = std::make_unique<UnixThreadProc>(loop, shared_from_this());
    channel = std::make_shared<Event::EventBufferChannel>(loop, wakeupChannelFd);
    channel->setEvents(EV_PERSIST);
    channel->setOnReadCallable(([this](auto && PH1, auto && PH2)
    {
        std::cout << "onTask" << std::endl;
        queue->dispatchTask(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); }));
    // //开启读取事件
    channel->enableReading(-1);
    threadHandle = std::make_unique<std::thread>(ThreadProc, std::move(proc), shared_from_this());

    mRunStatus = true;
    return true;
}
}
}
